#include <charconv>
#include <iostream>
#include <memory>
#include <string_view>
#include "clidefs.h"
#include "Discarder.h"
#include "Match.h"
#include "GreedyPlayer.h"
#include "MinimaxPlayer.h"
#include "RandomPlayer.h"

using DiscarderPtr = std::unique_ptr<Discarder>;
using PlayerPtr = std::unique_ptr<Player>;

DiscarderPtr build_discarder(char descriptor)
{
   switch (descriptor) {
      case 'r':
         return std::make_unique<RandomDiscarder>();
         break;

      case 'g':
         return std::make_unique<GreedyDiscarder>();
         break;

      case 's':
         return std::make_unique<TableDiscarder>(disc_net_show_dat);
         break;

      case 'h':
         return std::make_unique<TableDiscarder>(disc_net_hand_dat);
         break;

      default:
         break;
   }

   return nullptr;
}

PlayerPtr build_player(char descriptor, Discarder& discarder)
{
   switch (descriptor) {
      case 'r':
         return std::make_unique<RandomPlayer>(discarder);
         break;

      case 'g':
         return std::make_unique<GreedyPlayer>(discarder);
         break;

      case 'm':
         return std::make_unique<MinimaxPlayer>(discarder);
         break;

      default:
         break;
   }

   return nullptr;
}

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

int show_usage()
{
   std::cout
      << "Usage: play_match <player 1> <player 2> <number of games>\n"
      << "Players are specified by two characters, the first indicating the discard\n"
      << "strategy and the second the card play strategy.\n"
      << "Discard strategies:\n"
      << "    r - Random\n"
      << "    g - Greedy\n"
      << "    s - Maximize expected net points scored during the show only\n"
      << "    h - Maximize expected net points scored during the entire hand\n"
      << "Card play strategies:\n"
      << "    r - Random\n"
      << "    g - Greedy\n"
      << "    m - Monte Carlo minimax\n"
      << "Example: play_match gg hm 1000\n"
      << std::endl;

   return -1;
}

int main(int argc, char* const argv[])
{
   if (argc != 4) {
      return show_usage();
   }

   auto desc1 = argv[1];
   auto desc2 = argv[2];
   auto sz_games = argv[3];

   if ((strlen(desc1) != 2) || (strlen(desc2) != 2)) {
      return show_usage();
   }

   DiscarderPtr discarder1 = build_discarder(desc1[0]);
   DiscarderPtr discarder2 = build_discarder(desc2[0]);
   if (!discarder1 || !discarder2) {
      return show_usage();
   }

   PlayerPtr player1 = build_player(desc1[1], *discarder1);
   PlayerPtr player2 = build_player(desc2[1], *discarder2);
   if (!player1 || !player2) {
      return show_usage();
   }

   int games = 0;
   if (!get_arg_value(sz_games, games)) {
      return show_usage();
   }

   Match match({ player1.get(), player2.get() });
   MatchResults results = match.play(games, true);

   std::cout << "Player 1: " << results.wins[0] << " wins" << std::endl;
   std::cout << "Player 2: " << results.wins[1] << " wins" << std::endl;

   return 0;
}
