//
// Copyright 2022 Stephen E. Bensley
//
// This file is licensed under the MIT License. You may obtain a copy of the
// license at https://github.com/stephenbensley/Tensity/blob/main/LICENSE.
//

#ifndef ExpectimaxPlayer_h
#define ExpectimaxPlayer_h

#include "Discarder.h"
#include "MinimaxStrategy.h"
#include "Player.h"

// Uses minimax strategy for card play.
class MinimaxPlayer : public Player
{
public:
   explicit MinimaxPlayer(Discarder& discarder);
   virtual std::unique_ptr<Player> clone() const override;
   virtual CardsDiscarded get_discards(const GameView& game,
                                       const CardsInHand& hand) override;
   virtual Card get_card_to_play(const GameView& game,
                                 const CardsInHand& hand) override;

   virtual void on_starter_revealed(const GameView& game,
                                    Card starter,
                                    int points) override;
   virtual void on_play(const GameView& game,
                        Play play,
                        int points) override;
   virtual void on_set_index(PlayerIndex new_value) override;

private:
   Discarder& discarder_;
   MinimaxStrategy card_play_;
};

inline MinimaxPlayer::MinimaxPlayer(Discarder& discarder)
: discarder_(discarder)
{ }

#endif /* ExpectimaxPlayer_h */
