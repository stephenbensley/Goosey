#include <fstream>
#include <iostream>
#include <limits>
#include <string>
#include "clidefs.h"
#include "BoardValue.h"
#include "DiscardSimulator.h"
#include "Match.h"
#include "MinimaxPlayer.h"
#include "ScoreLogger.h"

// Converts the raw board value data into a comma-delimited file suitable for
// importing into a spreadsheet.
int gen_board_value_csv()
{
   std::ofstream ostrm(board_value_csv, std::ios::trunc);

   BoardValue board;
   if (!board.load(board_value_dat)) {
      std::cerr << "Failed to load " << board_value_dat << std::endl;
      return -1;
   }

   for (auto i = 0; i < num_points_to_win; ++i) {
      ostrm << ", " << i;
   }
   ostrm << std::endl;

   for (auto i = 0; i < num_points_to_win; ++i) {
      ostrm << i;
      for (auto j = 0; j < num_points_to_win; ++j) {
         ostrm << ", " << board[i][j];
      }
      ostrm << std::endl;
   }

   return 0;
}

// Generates a table of win probabilities for each board position.
int gen_board_value_dat()
{
   ScoreLog scorelog;
   if (!scorelog.load(score_log_dat)) {
      std::cerr << "Failed to load " << score_log_dat << std::endl;
      return -1;
   }

   BoardValue board;
   board.build(scorelog);
   board.save(board_value_dat);
   return 0;
}

// Shared function for generating a discard table. If use_hvh is false, it
// computes the best strategy considering only the show, i.e., it doesn't
// consider points scored during card play.
int gen_disc_dat(bool use_hvh)
{
   DiscardTable strategy;
   if (use_hvh && strategy.load(disc_net_hand_dat)) {
      std::cout << "Starting with existing " << disc_net_hand_dat << std::endl;
   } else if (strategy.load(disc_net_show_dat)) {
      std::cout << "Starting with existing " << disc_net_show_dat << std::endl;
   } else {
      strategy = generate_greedy_strategy();
      std::cout << "Starting with greedy strategy." << std::endl;
   }

   HandVsHand hvh;
   if (use_hvh && !hvh.load(hand_vs_hand_dat)) {
      std::cerr << "Failed to load " << hand_vs_hand_dat << std::endl;
      return -1;
   }

   auto iteration = 0;
   auto lowest_exploit = std::numeric_limits<double>::max();

   while (true) {
      DiscardSimulator simulator(strategy, hvh);
      std::cout << "Iteration: " << iteration << std::endl;
      simulator.simulate(10'000'000'000);
      auto exploit = simulator.best_response(strategy);
      std::cout << "Exploitability: " << exploit << std::endl;

      if (exploit >= lowest_exploit) {
         std::cout << "No improvement -- exiting." << std::endl;
         break;
      }

      // Save our new best strategy.
      strategy.save(use_hvh ? disc_net_hand_dat : disc_net_show_dat);

      lowest_exploit = exploit;
      ++iteration;
   }

   return 0;
}

// Generates the discard table that maximizes the expected net points scored
// during the show (i.e., ignoring points scored during card play).
int gen_disc_net_hand_dat()
{
   return gen_disc_dat(true);
}

// Generates the discard table that maximizes the expected net points scored
// during the entire hand (i.e., including points scored during card play).
int gen_disc_net_show_dat()
{
   return gen_disc_dat(false);
}

// Generates the table of outcomes for all possible combinations of card play
// hands.
int gen_hand_vs_hand_dat()
{
   HandVsHand hvh;
   hvh.build();
   hvh.save(hand_vs_hand_dat);
   return 0;
}

// Generates a log containing the sequence of scores for a large number of
// games. Useful for high-speed simulation of Cribbage games.
int gen_score_log_dat()
{
   const int target_hands = 1'000'000;

   // Use the strongest player strategy for the matches.
   TableDiscarder discarder(disc_net_hand_dat);
   MinimaxPlayer player0(discarder), player1(discarder);
   // Wrap one of the players in a score logger.
   ScoreLogger logger(player0);

   Match match({ &logger, &player1 });
   // There are an average of ~8.5 hands per match, so playing target_hands/8
   // matches should give us enough hands. This is a bit clunky, but it works,
   // and it's not worth the effort to do something more elegant.
   //
   // Turn off symmetric play since we want maximum entropy in our score log.
   match.play(target_hands/8, false);

   // Make sure we did indeed play enough hands.
   auto actual_hands = logger.log().size();
   if (actual_hands < target_hands) {
      std::cerr << "Only played " << actual_hands << " hands." << std::endl;
      return -1;
   }

   // Throw away any excess hands before saving the result.
   logger.log().resize(target_hands);
   logger.log().save(score_log_dat);

   return 0;
}

int show_usage()
{
   std::cout
      << "Usage: gen_file <filename>\n"
      << "\n"
      << "Valid filenames:\n"
      << "   " << board_value_csv << "\n"
      << "   " << board_value_dat << "\n"
      << "   " << disc_net_hand_dat << "\n"
      << "   " << disc_net_show_dat << "\n"
      << "   " << hand_vs_hand_dat << "\n"
      << "   " << score_log_dat << "\n"
      << "\n"
      << "Example: gen_file disc_net_hand.dat\n"
      << std::endl;

   return -1;
}

int main(int argc, char* const argv[])
{
   if (argc != 2) {
      return show_usage();
   }

   std::string filename(argv[1]);
   if (filename == board_value_csv) {
      return gen_board_value_csv();
   } else if (filename == board_value_dat) {
      return gen_board_value_dat();
   } else if (filename == disc_net_hand_dat) {
      return gen_disc_net_hand_dat();
   } else if (filename == disc_net_show_dat) {
      return  gen_disc_net_show_dat();
   } else if (filename == hand_vs_hand_dat) {
      return gen_hand_vs_hand_dat();
   } else if (filename == score_log_dat) {
      return gen_score_log_dat();
   }

   return show_usage();
}
