#include "DiscardSimulator.h"
#include "Canonize.h"
#include "DiscardAnalyzer.h"
#include "FileIO.h"
#include <algorithm>
#include <future>
#include <limits>
#include <vector>

DiscardSimulator::ActionResult&
DiscardSimulator::ActionResult::operator+=(const ActionResult& rhs) noexcept
{
   observer_play_points += rhs.observer_play_points;
   observer_hand_points += rhs.observer_hand_points;
   opponent_play_points += rhs.opponent_play_points;
   opponent_hand_points += rhs.opponent_hand_points;
   return *this;
}

int DiscardSimulator::ActionResult::observer_net_points() const noexcept
{
   return (observer_play_points + observer_hand_points) -
          (opponent_play_points + opponent_hand_points);
}

DiscardSimulator::DiscardSimulator(const DiscardTable& opponent,
                                   const HandVsHand& hvh) noexcept
: opponent_(opponent),
  hvh_(hvh)
{
   // Create all the entries ahead of time, so the map will be read-only
   // during computation. This allows multiple workers to access the map
   // without locking.
   generate_cards_dealt([this](auto cards) {
      auto key = canonize(cards);
      auto i = entries_.find(key);
      if (i == entries_.end()) {
         entries_[key];
      }
   });
}

void DiscardSimulator::simulate(int64_t num_hands)
{
   auto concurrency = std::thread::hardware_concurrency();

   // Compute number of iterations each worker should perform.
   auto iter_by_worker_n = num_hands / concurrency;
   auto iter_by_worker_0 =
      num_hands - ((concurrency - 1) * iter_by_worker_n);

   // Launch the workers ...
   std::vector<std::future<void>> futures;
   for (auto i = 0; i < concurrency; ++i) {
      auto iter_by_worker = (i == 0) ? iter_by_worker_0 : iter_by_worker_n;
      futures.push_back(std::async(std::launch::async,
                                   &DiscardSimulator::simulate_worker,
                                   this,
                                   iter_by_worker));
   }
   // ... and wait for them to complete.
   std::for_each(futures.begin(), futures.end(), [](auto& f){ f.get(); });
}

double DiscardSimulator::best_response(DiscardTable& response) const noexcept
{
   response.clear();

   // Compute the best response for every entry. Use int64_t to avoid overflow.
   int64_t points_sum = 0;
   int64_t count_sum = 0;
   for (const auto& [key, entry] : entries_) {
      auto [d_action, d_points] = find_best(entry.dealer.results);
      auto [p_action, p_points] = find_best(entry.pone.results);
      points_sum += d_points + p_points;
      count_sum += entry.dealer.count + entry.pone.count;
      response.insert(key, d_action, p_action);
   }

   // Compute the overall exploitability.
   return static_cast<double>(points_sum) / static_cast<double>(count_sum);
}

// Helper functions to compute the average number of milli-points.
int16_t avg_mpoints(int sum_points, int count) noexcept
{
   assert(sum_points >= 0);
   assert(count > 0);
   return (1000l * sum_points + 500l) / count;
}

ExpectedScores::Scores
DiscardSimulator::State::expected_scores() const noexcept
{
   ExpectedScores::Scores scores;
   if (count == 0) { return scores; }

   for (auto i = 0; i < num_discard_actions; ++i) {
      auto& src = results[i];
      auto& dst = scores[i];

      dst.observer_play_mpoints = avg_mpoints(src.observer_play_points, count);
      dst.observer_hand_mpoints = avg_mpoints(src.observer_hand_points, count);
      dst.opponent_play_mpoints = avg_mpoints(src.opponent_play_points, count);
      dst.opponent_hand_mpoints = avg_mpoints(src.opponent_hand_points, count);
   }

   return scores;
}

void DiscardSimulator::expected_scores(ExpectedScores& results) const noexcept
{
   results.clear();
   for (auto& [key, entry] : entries_) {
      results.insert(key, {
         entry.dealer.expected_scores(),
         entry.pone.expected_scores()
      });
   }
}

bool DiscardSimulator::load(const char* filename)
{
   std::ifstream istrm(filename, std::ios::binary);
   if (!istrm.is_open()) {
      return false;
   }
   MapType tmp;
   if (!read_pod_map(istrm, tmp)) {
      return false;
   }
   if (!read_complete(istrm)) {
      return false;
   }
   entries_.swap(tmp);
   return true;

}

void DiscardSimulator::save(const char* filename) const noexcept
{
   std::ofstream ostrm(filename, std::ios::binary | std::ios::trunc);
   write_pod_map(ostrm, entries_);
}

void DiscardSimulator::simulate_worker(int64_t num_hands) noexcept
{
   Deck deck;
   while (num_hands-- > 0) {
      deck.shuffle();

      // Deal the hands and the starter card.
      DiscardAnalyzer opponent(deck);
      DiscardAnalyzer observer(deck);
      const auto starter = deck.deal_card();

      // Look up the corresponding state for each hand.
      auto actions = opponent_.find(opponent.key());
      Entry& entry = entries_[observer.key()];

      // Evaluate the hands both ways. It's more efficient to do both at once
      // since all the processing above is shared.
      for (auto dealer : { false, true }) {
         // Opponent always takes a single action.
         opponent.take_action(dealer ? actions.pone : actions.dealer);
         auto opponent_hand_points = opponent.hand_points(starter);
         auto opponent_ordinal = hvh_.ordinal(opponent.kept());

         // Results are calculated for all possible observer actions.
         ActionResults results;
         for (auto a = 0; a < num_discard_actions; ++a) {
            observer.take_action(a);
            auto observer_hand_points = observer.hand_points(starter);
            auto observer_ordinal = hvh_.ordinal(observer.kept());

            // Doesn't matter which way we compute crib points.
            auto crib_points = observer.crib_points(opponent.discarded(),
                                                    starter);
            assert(crib_points == opponent.crib_points(observer.discarded(),
                                                       starter));

            auto& result = results[a];
            if (dealer) {
               auto& cell = hvh_[observer_ordinal][opponent_ordinal];
               result.observer_play_points = cell.dealer_points;
               result.observer_hand_points = observer_hand_points + crib_points;
               result.opponent_play_points = cell.pone_points;
               result.opponent_hand_points = opponent_hand_points;
            } else {
               auto& cell = hvh_[opponent_ordinal][observer_ordinal];
               result.observer_play_points = cell.pone_points;
               result.observer_hand_points = observer_hand_points;
               result.opponent_play_points = cell.dealer_points;
               result.opponent_hand_points = opponent_hand_points + crib_points;
            }
         }

         // Now that all computations are complete, grab the lock and update
         // the shared state.
         auto& state = dealer ? entry.dealer : entry.pone;
         SpinlockGuard guard(state.lock);
         ++(state.count);
         for (auto a = 0; a < num_discard_actions; ++a) {
            state.results[a] += results[a];
         }
      }
   }
}

std::pair<int, int>
DiscardSimulator::find_best(const ActionResults& results) noexcept
{
   auto max_points = std::numeric_limits<int>::min();
   int best_action = 0;
   for (auto a = 0; a < num_discard_actions; ++a) {
      auto points = results[a].observer_net_points();
      if (points > max_points) {
         max_points = points;
         best_action = a;
      }
   }
   return { best_action, max_points };
}
