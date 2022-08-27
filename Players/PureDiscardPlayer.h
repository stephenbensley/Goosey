#ifndef PureDiscardPlayer_h
#define PureDiscardPlayer_h

#include "DiscardTable.h"
#include "GreedyPlayer.h"

// Extends GreedPlayer to implement an arbitrary pure discard strategy.
class PureDiscardPlayer : public GreedyPlayer
{
public:
   PureDiscardPlayer() noexcept;
   virtual std::unique_ptr<Player> clone() const override;
   virtual CardsDiscarded get_discards(const GameView& view,
                                       const CardsInHand& hand) override;

private:
   static constexpr char filename[] = "discard.dat";
   DiscardTable strategy_;
};

#endif /* PureDiscardPlayer_h */
