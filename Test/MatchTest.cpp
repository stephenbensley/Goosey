#include "Catch.hpp"
#include "Match.h"
#include "GreedyPlayer.h"
#include "RandomPlayer.h"
#include <algorithm>

TEST_CASE("Match::play(random vs. random)", "[match]")
{
   const int expected_wins = 100;
   const int num_games = expected_wins * num_players;

   RandomPlayer player0, player1;
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

   GreedyPlayer player0, player1;
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

   GreedyPlayer greedy;
   RandomPlayer random;
   Match match({ &greedy, &random });
   MatchResults results = match.play(num_games);
   const auto& wins = results.wins;

   int total = 0;
   std::for_each(wins.begin(),
                 wins.end(),
                 [&total](auto w){ total += w; });

   REQUIRE(total == num_games);
   REQUIRE(wins[0] > num_games * 9/10);
}
