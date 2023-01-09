//
// Copyright 2022 Stephen E. Bensley
//
// This file is licensed under the MIT License. You may obtain a copy of the
// license at https://github.com/stephenbensley/Goosey/blob/main/LICENSE.
//

#ifndef RandomPlayer_h
#define RandomPlayer_h

#include "Discarder.h"
#include "Player.h"

// Implements a Player that plays randomly.
class RandomPlayer : public Player
{
public:
   explicit RandomPlayer(Discarder& discarder);
   virtual std::unique_ptr<Player> clone() const override;
   virtual CardsDiscarded get_discards(const GameView& game,
                                       const CardsInHand& hand) override;
   virtual Card get_card_to_play(const GameView& game,
                                 const CardsInHand& hand) override;
   virtual void on_set_index(PlayerIndex new_value) override;

private:
   Discarder& discarder_;
};

inline RandomPlayer::RandomPlayer(Discarder& discarder)
: discarder_(discarder)
{ }

#endif /* RandomPlayer_h */
