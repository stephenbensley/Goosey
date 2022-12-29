//
// Copyright 2022 Stephen E. Bensley
//
// This file is licensed under the MIT License. You may obtain a copy of the
// license at https://github.com/stephenbensley/Tensity/blob/main/LICENSE.
//

#include "CardPlayModel.h"
#include "Score.h"

int CardPlayModel::play_rank(Rank rank) noexcept
{
   if (rank == go_rank) {
      return announce_go();
   }

   assert(is_legal_play(rank));

   ++cards_played_;
   count_ += rank_value(rank);
   auto points = score_.update_and_score(rank);

   if (count_ == max_count_in_play) {
      start_new_series();
   } else if (round_over()) {
      points += num_points_for_last_card;
   }

   if (!go_announced_) {
      next_player();
   }

   return points;
}

void CardPlayModel::start_new_round(PlayerIndex dealer) noexcept
{
   cards_played_ = 0;
   current_ = ::next_player(dealer);
   start_new_series();
}

int CardPlayModel::announce_go() noexcept
{
   auto points = 0;

   // If everyone has announced go, then no one can play a card, and the series
   // is over. The last player to announce go must also have played the last
   // card, so they score points for last card.
   if (go_announced_) {
      start_new_series();
      points = num_points_for_last_card;
   } else {
      go_announced_ = true;
   }

   next_player();
   return points;
}
