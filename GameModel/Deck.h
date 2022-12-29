//
// Copyright 2022 Stephen E. Bensley
//
// This file is licensed under the MIT License. You may obtain a copy of the
// license at https://github.com/stephenbensley/Tensity/blob/main/LICENSE.
//

#ifndef Deck_h
#define Deck_h

#include "Card.h"
#include "SizedArray.h"
#include <cassert>
#include "pcg_random.hpp"

// Represents a deck of playing cards.
class Deck
{
public:
   Deck() noexcept;
   Deck(const Deck& other) noexcept;
   Deck& operator=(const Deck& rhs) noexcept;

   bool empty() const noexcept;
   // Number of cards remaining in the deck (i.e., that haven't been dealt).
   int size() const noexcept;

   // Returns all cards to the deck and shuffles. 'num_cards' is a performance
   // optimization; no need to shuffle more cards than we need for a round.
   void shuffle(int num_cards = num_cards_dealt_per_round) noexcept;
   // Removes and returns the top card.
   Card deal_card() noexcept;

private:
   SizedArray<Card, num_cards_in_deck> cards_;
   const Card* next_;
   pcg32 rng_;
};

// Helper function to deal a full hand from the deck.
CardsDealt deal_cards(Deck& deck) noexcept;

inline bool Deck::empty() const noexcept
{
   return cards_.begin() == next_;
}

inline int Deck::size() const noexcept
{
   return static_cast<int>(std::distance(cards_.begin(), next_));
}

inline Card Deck::deal_card() noexcept
{
   assert(!empty());
   return *--next_;
}

#endif /* Deck_h */
