#include "Match.h"
#include "Deck.h"
#include "GameController.h"
#include <chrono>
#include <future>

using namespace std::chrono;

MatchResults& MatchResults::operator+=(const MatchResults rhs) noexcept
{
   for (auto i = 0; i <= num_players; ++i) {
      wins[i] += rhs.wins[i];
   }
   return *this;
}

Match::Match(const Players& players) noexcept
: players_(players)
{
   for (auto i = 0; i < players.size(); ++i) {
      players[i]->set_index(i);
   }
}

MatchResults Match::play(int num_games, bool symmetric) const
{
   auto concurrency = std::thread::hardware_concurrency();

   // Ideally, each worker should execute a multiple of num_players. This allows
   // each player to get the same set of decks.
   auto games_by_worker_n =
      num_players * (num_games / (concurrency * num_players));
   // One worker may have to execute a different number of games to meet the
   // total game count.
   auto games_by_worker_0 =
      num_games - ((concurrency - 1) * games_by_worker_n);

   // MatchResults to be returned by each worker.
   std::vector<std::future<MatchResults>> results;

   auto start = high_resolution_clock::now();

   for (auto i = 0; i < concurrency; ++i) {
      auto num_games = (i == 0) ? games_by_worker_0 : games_by_worker_n;
      results.push_back(std::async(std::launch::async,
                                   &Match::play_worker,
                                   this,
                                   num_games,
                                   symmetric));
   }

   MatchResults total = { 0 };
   std::for_each(results.begin(),
                 results.end(),
                 [&total](auto& r){ total += r.get(); });

   auto stop = high_resolution_clock::now();
   auto duration = duration_cast<microseconds>(stop - start);
   duration /= num_games;
   total.usec_per_game = static_cast<int>(duration.count());
   total.core_usec_per_game = total.usec_per_game * concurrency;

   return total;
}

MatchResults Match::play_worker(int num_games, bool symmetric) const
{
   // Clone the players. We create a separate set for each worker, so the
   // players don't have to be thread-safe.
   std::array<std::unique_ptr<Player>, num_players> clones;
   std::transform(players_.begin(),
                  players_.end(),
                  clones.begin(),
                  [] (const auto& p) { return p->clone(); });

   // Convert the smart pointers to raw pointers.
   Players raw_players;
   std::transform(clones.begin(),
                  clones.end(),
                  raw_players.begin(),
                  [](const auto& p) { return p.get(); });

   // We save & restore the deck of cards, so each player gets the same deck for
   // their deal. This reduces the luck factor.
   Deck deck;
   Deck saved(deck);
   MatchResults results = { 0 };

   for (auto i = 0; i < num_games; ++i) {
      auto first_deal = (i % num_players);

      if (symmetric) {
         // On each lap through the players, the first player gets a fresh deck.
         // The remaining players get that same deck.
         if (first_deal == 0) {
            saved = deck;
         } else {
            deck = saved;
         }
      }

      GameController game(raw_players, deck, first_deal);
      ++results.wins[game.play()];
   }

   return results;
}
