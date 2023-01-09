//
// Copyright 2022 Stephen E. Bensley
//
// This file is licensed under the MIT License. You may obtain a copy of the
// license at https://github.com/stephenbensley/Goosey/blob/main/LICENSE.
//

#include "CardPlayNode.h"
#include <algorithm>

void CardPlayNode::start_new_round(const RanksInHand& hand,
                                   bool dealer) noexcept
{
   // Save the observer's hand.
   observer_.resize(hand.size());
   std::copy(hand.begin(), hand.end(), observer_.begin());

   // Resize the opponent's hand, but leave the contents alone. This will be
   // filled in when randomize is called.
   opponent_.resize(num_cards_in_hand);

   // Reset the CardPlayGame.
   model_.start_new_round(dealer ? observer_index
                                 : other_player(observer_index));

   result_ = 0;
}

void CardPlayNode::randomize(const RanksInHand& unseen) noexcept
{
   assert(unseen.size() == unseen_card_count());
   std::copy(unseen.begin(), unseen.end(), opponent_.begin());
}

RanksInHand CardPlayNode::valid_plays() const noexcept
{
   RanksInHand valid;
   for (auto rank : current_hand()) {
      // Check for duplicates because the hand may contain multiple cards of
      // the same rank.
      if (model_.is_legal_play(rank) && !valid.contains(rank)) {
         valid.push_back(rank);
      }
   }
   if (valid.empty()) {
      valid.push_back(go_rank);
   }
   return valid;
}

void CardPlayNode::make_play(Rank play, bool force) noexcept
{
   if (play != go_rank) {
      if (force) {
         assert(!is_current_player());
         current_hand().pop_back();
      } else {
         [[maybe_unused]] auto removed = current_hand().remove_first(play);
         assert(removed);
      }
   }
   // Save this before making a play since that may change current player.
   int color = is_current_player() ? +1 : -1;
   result_ += model_.play_rank(play) * color;
}
