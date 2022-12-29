#include "Catch.hpp"
#include "Match.h"
#include "GreedyPlayer.h"
#include "RandomPlayer.h"
#include <algorithm>

TEST_CASE("Match::play(random vs. random)", "[match]")
{
   const auto expected_wins = 100;
   const auto num_games = expected_wins * num_players;

   // With symmetric play, identical players should win exactly the same number
   // of games.
   RandomDiscarder discarder0, discarder1;
   RandomPlayer player0(discarder0), player1(discarder1);
   Match match({ &player0, &player1 });
   MatchResults results = match.play(num_games);
   auto wins = results.wins;

   REQUIRE(std::find_if(wins.begin(), wins.end(), [](auto w) {
      return w != expected_wins;
   }) == wins.end());
}

TEST_CASE("Match::play(greedy vs. greedy)", "[match]")
{
   const auto expected_wins = 100;
   const auto num_games = expected_wins * num_players;

   // With symmetric play, identical players should win exactly the same number
   // of games.
   GreedyDiscarder discarder0, discarder1;
   GreedyPlayer player0(discarder0), player1(discarder1);
   Match match({ &player0, &player1 });
   MatchResults results = match.play(num_games);
   auto wins = results.wins;

   REQUIRE(std::find_if(wins.begin(), wins.end(), [](auto w) {
      return w != expected_wins;
   }) == wins.end());
}

TEST_CASE("Match::play(greedy vs. random)", "[match]")
{
   const auto num_games = 1000;

   // Greedy player should crush random player.
   GreedyDiscarder discarder0;
   GreedyPlayer player0(discarder0);
   RandomDiscarder discarder1;
   RandomPlayer player1(discarder1);
   Match match({ &player0, &player1 });
   MatchResults results = match.play(num_games);
   auto wins = results.wins;

   REQUIRE(wins[0] > num_games * 9/10);
}
