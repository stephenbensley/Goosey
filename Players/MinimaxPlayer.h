#ifndef ExpectimaxPlayer_h
#define ExpectimaxPlayer_h

#include "DiscardStrategy.h"
#include "MinimaxStrategy.h"
#include "Player.h"

// Implements a pure table-lookup strategy for discards. Then uses expectimax
// for card play.
class MinimaxPlayer : public Player
{
public:
   MinimaxPlayer() noexcept;
   virtual std::unique_ptr<Player> clone() const override;
   virtual CardsDiscarded get_discards(const GameView& game,
                                       const CardsInHand& hand) override;
   virtual Card get_card_to_play(const GameView& game,
                                 const CardsInHand& hand) override;

   virtual void on_starter_revealed(const GameView& game,
                                    Card starter) override;
   virtual void on_opponent_play(const GameView& game,
                                 Play play) override;

private:
   static constexpr char filename[] = "discard.dat";
   PureDiscardStrategy discard_;
   MinimaxStrategy card_play_;
};

#endif /* ExpectimaxPlayer_h */
