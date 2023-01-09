//
// Copyright 2022 Stephen E. Bensley
//
// This file is licensed under the MIT License. You may obtain a copy of the
// license at https://github.com/stephenbensley/Goosey/blob/main/LICENSE.
//

#include "GameModel.h"
#include "HandScore.h"
#include <algorithm>


GameModel::GameModel(PlayerIndex first_deal) noexcept
: GameView(first_deal), card_play_(first_deal)
{ }

GameModel::Result GameModel::reveal_starter(Card card) noexcept
{
   set_starter(card);
   // If the starter is a jack, dealer scores for 'his heels'.
   if (starter().is_jack()) {
      return get_result(dealer(), num_points_for_his_heels);
   } else {
      return { 0, false };
   }
}

GameModel::Result GameModel::play_card(Card card) noexcept
{
   auto player = current_player();
   GameView::play_card({ player, card });
   auto points = card_play_.play_rank(card.rank());
   if (card_play_.count() == 0) {
      start_new_series();
   }
   return get_result(player, points);
}

GameModel::Result GameModel::show_pone() noexcept
{
   return get_result(pone(), score_hand(pone()));
}

GameModel::Result GameModel::show_dealer() noexcept
{
   return get_result(dealer(), score_hand(dealer()));
}

GameModel::Result GameModel::show_crib(const CardsShown& crib) noexcept
{
   set_crib(crib);
   return get_result(dealer(), score_crib());
}

void GameModel::start_new_round() noexcept
{
   GameView::start_new_round();
   card_play_.start_new_round(dealer());
}

int GameModel::score_hand(PlayerIndex player) const noexcept
{
   return HandScore(hand(player).begin(),
                    hand(player).end(),
                    false).score(starter());
}

int GameModel::score_crib() const noexcept
{
   return HandScore(crib().begin(), crib().end(), true).score(starter());
}
