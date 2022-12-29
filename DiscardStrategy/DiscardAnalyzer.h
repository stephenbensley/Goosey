//
// Copyright 2022 Stephen E. Bensley
//
// This file is licensed under the MIT License. You may obtain a copy of the
// license at https://github.com/stephenbensley/Tensity/blob/main/LICENSE.
//

#ifndef DiscardAnalyzer_h
#define DiscardAnalyzer_h

#include "Card.h"
#include "CardSplitter.h"
#include "Deck.h"

// Analyzes the values of different discard options for a hand.
class DiscardAnalyzer
{
public:
   explicit DiscardAnalyzer(Deck& deck) noexcept;

   // Returns the canonical key for the hand.
   uint64_t key() const noexcept;

   // Set the current action for the hand. This sets the context for subsequent
   // calls to discards(), hand_points(), etc.
   void take_action(int action) noexcept;

   // Get the results of the last action selected.
   const CardsKept& kept() const noexcept;
   const CardsDiscarded& discarded() const noexcept;
   int hand_points(Card starter) const noexcept;
   int crib_points(const CardsDiscarded& opponent, Card starter) noexcept;
   int total_points(bool dealer,
                    const CardsDiscarded& opponent,
                    Card starter) noexcept;
   
private:
   CardsDealt cards_;
   uint64_t key_;
   CardSplitter splitter_;
};

inline uint64_t DiscardAnalyzer::key() const noexcept
{
   return key_;
}

inline void DiscardAnalyzer::take_action(int action) noexcept
{
   splitter_.seek(action);
}

inline const CardsKept& DiscardAnalyzer::kept() const noexcept
{
   return splitter_.hand;
}

inline const CardsDiscarded& DiscardAnalyzer::discarded() const noexcept
{
   return splitter_.crib;
}

#endif /* DiscardAnalyzer_h */
