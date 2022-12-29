//
// Copyright 2022 Stephen E. Bensley
//
// This file is licensed under the MIT License. You may obtain a copy of the
// license at https://github.com/stephenbensley/Tensity/blob/main/LICENSE.
//

#ifndef GreedyPlayer_h
#define GreedyPlayer_h

#include "Discarder.h"
#include "Player.h"

// Implements a player that chooses the play with the largest immediate reward.
class GreedyPlayer : public Player
{
public:
   explicit GreedyPlayer(Discarder& discarder);
   virtual std::unique_ptr<Player> clone() const override;
   virtual CardsDiscarded get_discards(const GameView& game,
                                       const CardsInHand& hand) override;
   virtual Card get_card_to_play(const GameView& game,
                                 const CardsInHand& hand) override;
   virtual void on_set_index(PlayerIndex new_value) override;

private:
   Discarder& discarder_;
};

inline GreedyPlayer::GreedyPlayer(Discarder& discarder)
: discarder_(discarder)
{ }

#endif /* GreedyPlayer_h */
