#include "Catch.hpp"
#include "Match.h"
#include "GreedyPlayer.h"
#include "RandomPlayer.h"
#include <algorithm>

TEST_CASE("Match::play(random vs. random)", "[match]")
{
   const int expected_wins = 100;
   const int num_games = expected_wins * num_players;

   RandomDiscarder discarder0, discarder1;
   RandomPlayer player0(discarder0), player1(discarder1);
   Match match({ &player0, &player1 });
   MatchResults results = match.play(num_games);
   const auto& wins = results.wins;

   int total = 0;
   std::for_each(wins.begin(),
                 wins.end(),
                 [&total](auto w){ total += w; });

   REQUIRE(total == num_games);

   REQUIRE(std::find_if(wins.begin(),
                        wins.end(),
                        [](auto w){ return w != expected_wins; }) ==
           wins.end());
}

TEST_CASE("Match::play(greedy vs. greedy)", "[match]")
{
   const int expected_wins = 100;
   const int num_games = expected_wins * num_players;

   GreedyDiscarder discarder0, discarder1;
   GreedyPlayer player0(discarder0), player1(discarder1);
   Match match({ &player0, &player1 });
   MatchResults results = match.play(num_games);
   const auto& wins = results.wins;

   int total = 0;
   std::for_each(wins.begin(),
                 wins.end(),
                 [&total](auto w){ total += w; });

   REQUIRE(total == num_games);

   REQUIRE(std::find_if(wins.begin(),
                        wins.end(),
                        [](auto w){ return w != expected_wins; }) ==
           wins.end());
}

TEST_CASE("Match::play(greedy vs. random)", "[match]")
{
   const int num_games = 1000;

   GreedyDiscarder discarder0;
   GreedyPlayer player0(discarder0);
   RandomDiscarder discarder1;
   RandomPlayer player1(discarder1);
   Match match({ &player0, &player1 });
   MatchResults results = match.play(num_games);
   const auto& wins = results.wins;

   int total = 0;
   std::for_each(wins.begin(),
                 wins.end(),
                 [&total](auto w){ total += w; });

   REQUIRE(total == num_games);
   REQUIRE(wins[0] > num_games * 9/10);
}
