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
   std::optional<double>
   compute_value(bool dealer,
                 int observer_score,
                 int opponent_score,
                 double his_heels,
                 const ExpectedScores::Score& score) const noexcept;

   BoardValue board_;
   ExpectedScores scores_;
};

#endif /* BoardPosDiscarder_h */
