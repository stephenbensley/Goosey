//
// Copyright 2022 Stephen E. Bensley
//
// This file is licensed under the MIT License. You may obtain a copy of the
// license at https://github.com/stephenbensley/Goosey/blob/main/LICENSE.
//

#include "Discarder.h"
#include "Canonize.h"
#include "FileIO.h"
#include "Score.h"

void Discarder::set_index(PlayerIndex new_value)
{
   assert(index_ == invalid_player);
   index_ = new_value;
}

CardSplitter RandomDiscarder::get_discards(const GameView& game,
                                           const CardsInHand& hand)
{
   return CardSplitter(hand.data());
}

CardSplitter GreedyDiscarder::get_discards(const GameView& game,
                                           const CardsInHand& hand)
{
   CardSplitter splitter(hand.data());
   // Crib counts for the dealer and against the pone.
   auto crib_mult = is_dealer(game) ? +1 : -1;

   auto max_points = std::numeric_limits<int>::min();
   auto best_pos = -1;

   do  {
      auto hand_points = score_hand(splitter.hand.begin(),
                                    splitter.hand.end(),
                                    nullcard,
                                    false);
      auto crib_points = score_hand(splitter.crib.begin(),
                                    splitter.crib.end(),
                                    nullcard,
                                    true);
      auto points = hand_points + crib_mult * crib_points;
      if (points > max_points) {
         max_points = points;
         best_pos = splitter.pos();
       }
   } while (splitter.next());

   assert(best_pos >= 0);
   splitter.seek(best_pos);
   return splitter;
}

TableDiscarder::TableDiscarder(const char* datafile)
{
   if (!strategy_.load(datafile)) {
      throw FileNotFound();
   }
}

CardSplitter TableDiscarder::get_discards(const GameView& game,
                                          const CardsInHand& hand)
{
   CardsDealt canonical(hand.data());
   auto actions = strategy_.find(canonize(canonical));
   CardSplitter splitter(canonical);
   splitter.seek(is_dealer(game) ? actions.dealer : actions.pone);
   return splitter;
}



