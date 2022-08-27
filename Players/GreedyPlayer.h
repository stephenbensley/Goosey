#ifndef GreedyPlayer_h
#define GreedyPlayer_h

#include "Player.h"

// Implements a player that chooses the play with the largest immediate reward.
class GreedyPlayer : public Player
{
public:
   virtual std::unique_ptr<Player> clone() const override;
   virtual CardsDiscarded get_discards(const GameView& game,
                                       const CardsInHand& hand) override;
   virtual Card get_card_to_play(const GameView& game,
                                 const CardsInHand& hand) override;
};

#endif /* GreedyPlayer_h */
