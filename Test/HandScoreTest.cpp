//
// Copyright 2022 Stephen E. Bensley
//
// This file is licensed under the MIT License. You may obtain a copy of the
// license at https://github.com/stephenbensley/Goosey/blob/main/LICENSE.
//

#include "Catch.hpp"
#include "HandScore.h"
#include "Deck.h"
#include "Score.h"
#include <algorithm>

TEST_CASE("HandScore", "[score]")
{
   // Veryify randomly generated hands are scored the same using either
   // manual scoring or cachec scoring.
   const auto num_hands = 1000;
   auto num_passsed = 0;

   Deck deck;
   CardsShown hand;
   Card starter;

   for (auto j = 0; j < num_hands; ++j) {
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

   REQUIRE(num_hands == num_passsed);
}
