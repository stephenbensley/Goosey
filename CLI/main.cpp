#include "BoardPosDiscarder.h"
#include "BoardValue.h"
#include "DiscardSimulator.h"
#include "GreedyPlayer.h"
#include "HandVsHand.h"
#include "Match.h"
#include "MinimaxPlayer.h"
#include "RandomPlayer.h"
#include "ScoreLogger.h"
#include <charconv>
#include <fstream>
#include <iostream>
#include <string>

int verb_boardvalue(int argc, char* const argv[])
{
   ScoreLog scorelog;
   if (!scorelog.load("scorelog.dat")) {
      std::cout << "Failed to load scorelog.dat" << std::endl;
      return -1;
   }

   BoardValue board;
   board.build(scorelog);
   board.save("boardvalue.dat");

   std::ofstream ostrm("boardvalue.csv");
   for (auto i = 0; i < num_points_to_win; ++i) {
      for (auto j = 0; j < num_points_to_win; ++j) {
         if (j != 0) {
            ostrm << ", ";
         }
         ostrm << board[i][j];
      }
      ostrm << std::endl;
   }
   return 0;
}

int verb_discard(int argc, char* const argv[])
{
   DiscardTable strategy;
   if (!strategy.load("discard.dat")) {
      std::cout << "Failed to load discard.dat; using defaults." << std::endl;
      strategy = generate_greedy_strategy();
   }

   HandVsHand hvh;
   if (!hvh.load("hvh.dat")) {
      std::cout << "Failed to load hvh.dat" << std::endl;
      return -1;
   }

   for (auto i = 0; i < 99; ++i) {
      DiscardSimulator simulator(strategy, hvh);

      char filename[20];
      std::sprintf(filename, "response%02d.dat", i);

      std::cout << "Simulating " << filename << std::endl;
      simulator.simulate(10'000'000'000);
      auto exploitability = simulator.best_response(strategy);

      std::cout << "Exploitablity: " << exploitability << std::endl;
      strategy.save(filename);
   }

   return 0;
}

int verb_dumpboardvalue(int argc, char* const argv[])
{
   std::ofstream ostrm("boardvalue.csv", std::ios::trunc);

   BoardValue board;
   if (!board.load("boardvalue.dat")) {
      ostrm << "Failed to load boardvalue.dat" << std::endl;
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

int verb_expectedscores(int argc, char* const argv[])
{
   DiscardTable strategy;
   if (!strategy.load("discard.dat")) {
      std::cout << "Failed to load hvh.dat" << std::endl;
      return -1;
   }

   HandVsHand hvh;
   if (!hvh.load("hvh.dat")) {
      std::cout << "Failed to load hvh.dat" << std::endl;
      return -1;
   }

   DiscardSimulator simulator(strategy, hvh);
   std::cout << "Beginning simulation ..." << std::endl;

   for (auto i = 0; i < 100; ++i) {
      simulator.simulate(1'000'000'000);
      std::cout << "Iteration " << (i + 1) << " complete." << std::endl;
      simulator.save("simulate.dat");
      DiscardTable tmp;
      auto exploitablity = simulator.best_response(tmp);
      std::cout << "Exploitability " << exploitablity << std::endl;
   }

   ExpectedScores scores;
   simulator.expected_scores(scores);
   scores.save("expectedscores.dat");

   return 0;
}

int verb_hvh(int argc, char* const argv[])
{
   HandVsHand hvh;
   hvh.build();
   hvh.save("hvh.dat");
   return 0;
}

int verb_match(int argc, char* const argv[])
{
   TableDiscarder discarder0("discard.dat");
   MinimaxPlayer player0(discarder0);

   BoardPositionDiscarder discarder1("boardvalue.dat", "expectedscores.dat");
   MinimaxPlayer player1(discarder1);

   Match match({ &player0, &player1 });
   MatchResults results = match.play(1000, true);

   std::cout << "Player 1: " << results.wins[0] << std::endl;
   std::cout << "Player 2: " << results.wins[1] << std::endl;
   std::cout << "usec per game " << results.usec_per_game << std::endl;
   std::cout << "core-usec per game " << results.core_usec_per_game << std::endl;

   return 0;
}

int verb_scorelog(int argc, char* const argv[])
{
   const int target_hands = 1'000'000;

   TableDiscarder discarder("discard.dat");
   MinimaxPlayer player0(discarder), player1(discarder);
   ScoreLogger logger(player0);

   Match match({ &logger, &player1 });
   match.play(target_hands/8, false);
   auto actual_hands = logger.log().size();

   if (actual_hands < target_hands) {
      std::cout << "Only generated " << actual_hands << " hands." << std::endl;
      return -1;
   }

   logger.log().resize(target_hands);
   logger.log().save("scorelog.dat");

   return 0;
}

struct Verb {
   const char* name;
   int (*fn)(int x, char* const []);
};

#define DECLARE_VERB(name) \
{ #name, verb_ ## name }

const Verb verbs[] = {
   DECLARE_VERB(boardvalue),
   DECLARE_VERB(discard),
   DECLARE_VERB(dumpboardvalue),
   DECLARE_VERB(expectedscores),
   DECLARE_VERB(hvh),
   DECLARE_VERB(match),
   DECLARE_VERB(scorelog)
};
constexpr int num_verbs = sizeof(verbs)/sizeof(Verb);

int show_usage() noexcept
{
   std::cout << "usage: Goosey <verb> ...\nValid verbs:" << std::endl;
   for (auto i = 0; i < num_verbs; ++i) {
      std::cout << "   " << verbs[i].name << std::endl;
   }
   return -1;
}

int main(int argc, char* const argv[])
{
   if (argc > 1) {
      std::string lower(argv[1]);
      std::transform(lower.begin(), lower.end(), lower.begin(), tolower);
      for (auto i = 0; i < num_verbs; ++i) {
         if (lower == verbs[i].name) {
            return verbs[i].fn((argc - 1), (argv + 1));
         }
      }
   }
   return show_usage();
}
