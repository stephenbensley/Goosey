//
// Copyright 2022 Stephen E. Bensley
//
// This file is licensed under the MIT License. You may obtain a copy of the
// license at https://github.com/stephenbensley/Tensity/blob/main/LICENSE.
//

#include "MinimaxPlayer.h"

std::unique_ptr<Player> MinimaxPlayer::clone() const
{
    return std::make_unique<MinimaxPlayer>(*this);
}

CardsDiscarded MinimaxPlayer::get_discards(const GameView& game,
                                           const CardsInHand& hand)
{
   auto splitter = discarder_.get_discards(game, hand);

   RanksInHand ranks;
   for (auto card : splitter.hand) {
      ranks.push_back(card.rank());
   }
   card_play_.start_new_round(ranks, is_dealer(game));
   for (auto card : splitter.crib) {
      card_play_.on_rank_seen(card.rank());
   }

   return splitter.crib;
}

Card MinimaxPlayer::get_card_to_play(const GameView& game,
                                     const CardsInHand& hand)
{
   auto rank = card_play_.get_rank_to_play();
   if (rank != 0) {
      auto i = std::find_if(hand.begin(), hand.end(), [rank](auto c) {
         return c.rank() == rank;
      });
      assert(i != hand.end());
      return *i;
   }
   return go_card;
}

void MinimaxPlayer::on_starter_revealed(const GameView& game,
                                        Card starter,
                                        int points)
{
   card_play_.on_rank_seen(starter.rank());
}

void MinimaxPlayer::on_play(const GameView& game, Play play, int points)
{
   if (play.player != index()) {
      card_play_.on_opponent_play(play.card.rank());
   }
}

void MinimaxPlayer::on_set_index(PlayerIndex new_value)
{
   discarder_.set_index(new_value);
}
