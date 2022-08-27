#include "MinimaxPlayer.h"
#include "GreedyPlayer.h"
#include "Match.h"
#include "PureDiscardPlayer.h"
#include "RandomPlayer.h"
#include <charconv>
#include <iostream>
#include <string_view>

// Converts a string argument to int. Returns true if the conversion succeeds.
// Leaves value unmodified if the conversion fails.
bool get_arg_value(const std::string_view& sv, int& value)
{
   int tmp;
   auto [end, ec] = std::from_chars(sv.begin(), sv.end(), tmp);
   if ((ec != std::errc()) || (end != sv.end())) {
      return false;
   }
   value = tmp;
   return true;
}

int match_main(int argc, char* const argv[])
{
   MinimaxPlayer player0, player1;
   Match match({ &player0, &player1 });
   MatchResults results = match.play(100);
   
   std::cout << "Player 1: " << results.wins[0] << std::endl;
   std::cout << "Player 2: " << results.wins[1] << std::endl;
   std::cout << "usec per game " << results.usec_per_game << std::endl;
   std::cout << "core-usec per game " << results.core_usec_per_game << std::endl;

   return 0;
}
