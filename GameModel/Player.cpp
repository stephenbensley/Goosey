//
// Copyright 2022 Stephen E. Bensley
//
// This file is licensed under the MIT License. You may obtain a copy of the
// license at https://github.com/stephenbensley/Tensity/blob/main/LICENSE.
//

#include "Player.h"

void Player::set_index(PlayerIndex new_value)
{
   assert(index_ == invalid_player);
   index_ = new_value;
   on_set_index(new_value);
}

void Player::on_set_index(PlayerIndex new_value)
{ }

void Player::on_starter_revealed(const GameView& game,
                                 Card starter,
                                 int points)
{ }

void Player::on_play(const GameView& game, Play play, int points)
{ }

void Player::on_hand_show(const GameView& game,
                          PlayerIndex player,
                          const CardsPlayed& hand,
                          int points)
{ }

void Player::on_crib_show(const GameView& game,
                          const CardsInCrib& crib,
                          int points)
{ }

void Player::on_game_over(const GameView& game, PlayerIndex winner)
{ }
