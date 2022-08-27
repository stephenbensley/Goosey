#include "DiscardBestResponse.h"
#include "Canonize.h"
#include "DiscardAnalyzer.h"
#include <algorithm>
#include <future>
#include <limits>
#include <vector>


DiscardBestResponse::DiscardBestResponse(const PureDiscardStrategy& strategy,
                                         const HandVsHand& hvh) noexcept
: strategy_(strategy),
  hvh_(hvh)
{
   // Create all the entries ahead of time, so the map will be read-only
   // during computation. This allows multiple workers to access the map
   // without locking.
   generate_cards_dealt([this](auto cards) {
      auto key = canonize(cards);
      auto i = results_.find(key);
      if (i == results_.end()) {
          results_[key].actions = DiscardActions(cards);
      }
   });
}

// Find the best action given the array of points scored. Returns the best
// action and the number of points scored by the action.
std::pair<int, int>
find_best(const DiscardActions& actions,
          const std::array<int, num_discard_actions>& points)
{
   int max_points = std::numeric_limits<int>::min();
   int best_action = 0;
   actions.for_each([&](auto a) {
      if (points[a] > max_points) {
         max_points = points[a];
         best_action = a;
      }
   });
   return { best_action, max_points };
}

void DiscardBestResponse::compute(uint64_t num_iterations)
{
   auto concurrency = std::thread::hardware_concurrency();

   // Compute number of iterations each worker should perform.
   auto iter_by_worker_n = num_iterations / concurrency;
   auto iter_by_worker_0 =
      num_iterations - ((concurrency - 1) * iter_by_worker_n);

   // Launch the workers ...
   std::vector<std::future<void>> futures;
   for (auto i = 0; i < concurrency; ++i) {
      auto iter_by_worker = (i == 0) ? iter_by_worker_0 : iter_by_worker_n;
      futures.push_back(std::async(std::launch::async,
                                   &DiscardBestResponse::compute_worker,
                                   this,
                                   iter_by_worker));
   }
   // ... and wait for them to complete.
   std::for_each(futures.begin(), futures.end(), [](auto& f){ f.get(); });

   // Compute the best response for every entry.
   uint64_t points_sum = 0;
   for (const auto& [key, entry] : results_) {
      auto [d_action, d_points] = find_best(entry.actions, entry.dealer.points);
      auto [p_action, p_points] = find_best(entry.actions, entry.pone.points);
      points_sum += d_points + p_points;
      response_.insert(key, d_action, p_action);
   }

   // Compute the overall exploitability.
   exploitability_ = points_sum;
   // We evaluate the hand two ways for each iteration.
   exploitability_ /= (2 * num_iterations);
}

void DiscardBestResponse::compute_worker(uint64_t num_iterations) noexcept
{
   Deck deck;
   while (num_iterations-- > 0) {
      deck.shuffle();

      // Deal the hands and the starter card.
      DiscardAnalyzer strategy(deck);
      DiscardAnalyzer response(deck);
      const auto starter = deck.deal_card();

      // Look up the corresponding state for each hand.
      auto actions = strategy_.get_actions(strategy.key());
      Entry& entry = results_[response.key()];

      // Evaluate the hands both ways. It's more efficient to do both at once
      // since all the processing above is shared.
      for (auto dealer : { false, true }) {
         // Narrow the state based on who's dealer.
         auto action = dealer ? actions.dealer : actions.pone;
         auto& state = dealer ? entry.pone : entry.dealer;

         // Strategy under evaluation always takes a single action.
         strategy.take_action(action);
         auto strategy_points = strategy.hand_points(starter);
         auto strategy_ordinal = hvh_.ordinal(strategy.kept());

         // Best response is calculated for all possible actions.
         std::array<int, num_discard_actions> points;
         entry.actions.for_each([&](auto a) {
            response.take_action(a);
            points[a] = response.total_points(!dealer,
                                              strategy.discarded(),
                                              starter);
            points[a] -= strategy_points;

            // Update points based on expected outcome of card play.
            auto response_ordinal = hvh_.ordinal(response.kept());
            if (dealer) {
               auto& cell = hvh_[strategy_ordinal][response_ordinal];
               points[a] += cell.pone_points - cell.dealer_points;
            } else {
               auto& cell = hvh_[response_ordinal][strategy_ordinal];
               points[a] += cell.dealer_points - cell.pone_points;
            }
          });

         // Now that all computations are complete, grab the lock and update
         // the shared state.
         SpinlockGuard guard(state.lock);
         entry.actions.for_each([&](auto a){ state.points[a] += points[a]; });
      }
   }
}
