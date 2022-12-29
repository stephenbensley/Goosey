//
// Copyright 2022 Stephen E. Bensley
//
// This file is licensed under the MIT License. You may obtain a copy of the
// license at https://github.com/stephenbensley/Tensity/blob/main/LICENSE.
//

#include "RandomPlayer.h"
#include <algorithm>

std::unique_ptr<Player> RandomPlayer::clone() const
{
    return std::make_unique<RandomPlayer>(*this);
}

CardsDiscarded RandomPlayer::get_discards(const GameView& game,
                                          const CardsInHand& hand)
{
   return discarder_.get_discards(game, hand).crib;
}

Card RandomPlayer::get_card_to_play(const GameView& game,
                                    const CardsInHand& hand)
{
   auto i = std::find_if(hand.begin(), hand.end(), [&game](auto c) {
      return is_legal_play(game, c);
   });

   return (i != hand.end()) ? *i : go_card;
}

void RandomPlayer::on_set_index(PlayerIndex new_value)
{
   discarder_.set_index(new_value);
}
