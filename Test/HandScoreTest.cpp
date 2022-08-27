#include "Catch.hpp"
#include "HandScore.h"
#include "Deck.h"
#include "Score.h"
#include <algorithm>

TEST_CASE("HandScore", "[score]")
{
   const int num_iterations = 1000;
   int num_passsed = 0;

   Deck deck;
   CardsShown hand;
   Card starter;

   for (auto j = 0; j < num_iterations; ++j) {
      deck.shuffle();
      std::generate(hand.begin(),
                    hand.end(),
                    [&deck](){ return deck.deal_card(); });
      starter = deck.deal_card();

      auto points = HandScore(hand.begin(), hand.end(), false).score(starter);
      auto expected = score_hand(hand.begin(), hand.end(), starter, false);

      if (points == expected) {
         ++num_passsed;
      }
   }

   REQUIRE(num_iterations == num_passsed);
}
