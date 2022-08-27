#ifndef BoardPosDiscarder_h
#define BoardPosDiscarder_h

#include "BoardValue.h"
#include "Discarder.h"
#include "ExpectedScores.h"

// Discarder that takes into account board position when making decisions.
class BoardPositionDiscarder : public Discarder
{
public:
   BoardPositionDiscarder(const char* boardvalue,
                          const char* expectedscores);

   virtual CardSplitter get_discards(const GameView& game,
                                     const CardsInHand& hand) override;

private:
   BoardValue board_;
   ExpectedScores scores_;
};

#endif /* BoardPosDiscarder_h */
