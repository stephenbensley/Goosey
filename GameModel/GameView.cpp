//
// Copyright 2022 Stephen E. Bensley
//
// This file is licensed under the MIT License. You may obtain a copy of the
// license at https://github.com/stephenbensley/Tensity/blob/main/LICENSE.
//

#include "GameView.h"
#include <algorithm>

GameView::GameView(PlayerIndex first_deal) noexcept
: dealer_(first_deal)
{ }

bool GameView::peg_points(PlayerIndex player, int points) noexcept
{
   assert(is_valid_player(player));
   assert(points >= 0);
   scores_[player] += points;
   if (scores_[player] >= num_points_to_win) {
      // In cribbage, excess points are ignored.
      scores_[player] = num_points_to_win;
      winner_ = player;
      return true;
   }
   return false;
}

void GameView::set_starter(Card new_starter) noexcept
{
   assert(starter_ == nullcard);
   starter_ = new_starter;
}

void GameView::play_card(Play play) noexcept
{
   assert(!starter_.is_null());
   // View does not reflect 'go' announcements.
   if (play.card != go_card) {
      // Log the play.
      plays_.push_back(play);
      // Update the running count.
      count_ += play.card.value();
      assert(count_ <= max_count_in_play);
      // Track which cards each player has played. This could be derived from
      // the plays log, but it's useful to have the cards broken out by player.
      hands_[play.player].push_back(play.card);
   }
}

void GameView::set_crib(const CardsShown& new_crib) noexcept
{
   assert(plays_.size() == max_cards_in_play);
   assert(crib_.empty());
   crib_.insert(new_crib.begin(), new_crib.end());
}

void GameView::start_new_series() noexcept
{
   series_offset_ = static_cast<int>(plays_.size());
   count_ = 0;
}

void GameView::start_new_round() noexcept
{
   assert(!crib_.empty());
   // Scores are left untouched, deal rotates to next player, everything else
   // is reset to its original state.
   dealer_ = next_player(dealer_);
   starter_ = nullcard;
   plays_.clear();
   series_offset_ = 0;
   count_ = 0;
   std::for_each(hands_.begin(), hands_.end(), [](auto& h) { h.clear(); });
   crib_.clear();
   winner_ = invalid_player;
}
