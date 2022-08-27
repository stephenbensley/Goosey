#include "DiscardAnalyzer.h"
#include "Canonize.h"
#include "HandScore.h"

DiscardAnalyzer::DiscardAnalyzer(Deck& deck) noexcept
: cards_(deal_cards(deck)),
  key_(canonize(cards_)),
  splitter_(cards_)
{ }

int DiscardAnalyzer::hand_points(Card starter) const noexcept
{
   return HandScore(splitter_.hand.begin(),
                    splitter_.hand.end(),
                    false).score(starter);
}

int DiscardAnalyzer::crib_points(const CardsDiscarded& opponent,
                                 Card starter) noexcept
{
   HandScore crib(splitter_.crib.begin(), splitter_.crib.end(), true);
   // Add the crib cards from the opponent.
   crib.update(opponent.begin(), opponent.end());
   return crib.score(starter);
}

int DiscardAnalyzer::total_points(bool dealer,
                                  const CardsDiscarded& opponent,
                                  Card starter) noexcept
{
   // Crib counts for dealer and against the pone.
   const int crib_mult = dealer ? +1 : -1;
   auto hand_score = hand_points(starter);
   auto crib_score = crib_points(opponent, starter);
   return hand_score + (crib_mult * crib_score);
}

