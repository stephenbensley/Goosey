//
// Copyright 2022 Stephen E. Bensley
//
// This file is licensed under the MIT License. You may obtain a copy of the
// license at https://github.com/stephenbensley/Goosey/blob/main/LICENSE.
//

#ifndef CardSplitter_h
#define CardSplitter_h

#include "Card.h"
#include "DiscardDefs.h"

// Lists all possible ways to split cards between hand and crib.
class CardCombos
{
public:
   // Indices of cards for each possible combo.
   int hand[num_discard_actions][num_cards_in_hand];
   int crib[num_discard_actions][num_cards_discarded_per_player];

   static const CardCombos& get() noexcept;

private:
   CardCombos() noexcept;
};

// Enumerates all possible ways to discard from a given set of cards.
class CardSplitter
{
public:
   explicit CardSplitter(const CardsDealt& cards) noexcept;

   // Advance to the next combo. Returns false if there are no more combos.
   bool next() noexcept;

   // Returns the position of the current combo.
   int pos() const noexcept;
   // Skips to the specified combo.
   void seek(int pos) noexcept;

   // Full set of cards dealt.
   CardsDealt cards;
   // Set of cards to keep in the hand.
   CardsKept hand;
   // Set of cards to discard to the crib.
   CardsDiscarded crib;

private:
   // Loads the hand & crib with the cards for the given value of pos.
   void load() noexcept;

   const CardCombos& combos_ = CardCombos::get();
   int pos_ = 0;
};

inline int CardSplitter::pos() const noexcept
{
   return pos_;
}

inline void CardSplitter::seek(int pos) noexcept
{
   pos_ = pos;
   load();
}

#endif /* CardSplitter_h */
