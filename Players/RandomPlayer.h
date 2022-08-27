#ifndef RandomPlayer_h
#define RandomPlayer_h

#include "Player.h"

// Implements a Player that plays randomly.
class RandomPlayer : public Player
{
public:
   virtual std::unique_ptr<Player> clone() const override;
   virtual CardsDiscarded get_discards(const GameView& game,
                                       const CardsInHand& hand) override;
   virtual Card get_card_to_play(const GameView& game,
                                 const CardsInHand& hand) override;
};

#endif /* RandomPlayer_h */
