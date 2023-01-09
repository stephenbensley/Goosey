//
// Copyright 2022 Stephen E. Bensley
//
// This file is licensed under the MIT License. You may obtain a copy of the
// license at https://github.com/stephenbensley/Goosey/blob/main/LICENSE.
//

#ifndef Card_h
#define Card_h

#include "CribDefs.h"
#include "SizedArray.h"
#include <algorithm>
#include <array>
#include <cassert>
#include <string>
#include <string_view>
#include <vector>

// Represents the rank of a playing card, e.g., ace, deuce, etc.
using Rank = char;

// In cribbage, all face cards count as 10, so a card's value is not
// necessarily the same as its rank.
int rank_value(Rank rank) noexcept;

// Useful for indexing into an array when storing state about each rank.
int rank_ordinal(Rank rank) noexcept;

bool rank_is_valid(Rank rank) noexcept;

// Jacks are special for scoring.
constexpr Rank jack = card_rank_of_jack;

// It's convenient to use a zero rank to announce 'go'.
static_assert(min_card_rank > 0);
constexpr Rank go_rank = 0;

using RanksInHand = SizedArray<Rank, num_cards_in_hand>;

// Converts ranks to strings.
std::string to_string(Rank r);
std::string to_string(const Rank* begin, const Rank* end);

// Represents the suit of a playing card, e.g., spade, heart, etc.
using Suit = char;

bool suit_is_valid(Suit suit) noexcept;

// Represents a playing card.
class Card
{
public:
   constexpr Card() noexcept;
   // Each card can be identified by an ordinal in the range
   // [0, num_cards_in_deck). Useful for iterating through a deck of cards.
   explicit Card(int ordinal) noexcept;
   Card(Rank rank, Suit suit) noexcept;

   Rank rank() const noexcept;
   Suit suit() const noexcept;
   // In cribbage, all face cards count as 10, so a card's value is not
   // necessarily the same as its rank.
   int value() const noexcept;
   // Jacks are special for scoring.
   bool is_jack() const noexcept;
   // Checks if this is a null card. Useful for indicating that a card hasn't
   // been initialized or specified.
   bool is_null() const noexcept;

private:
   Rank rank_;
   Suit suit_;
};

// Fixed-size groups of cards
using CardsDealt = std::array<Card, num_cards_dealt_per_player>;
using CardsKept = std::array<Card, num_cards_in_hand>;
using CardsDiscarded = std::array<Card, num_cards_discarded_per_player>;
using CardsShown = std::array<Card, num_cards_in_show>;

// Variable-size groups of cards
using CardsInHand = SizedArray<Card, num_cards_dealt_per_player>;
using CardsInCrib = SizedArray<Card, num_cards_in_crib>;
using CardsInPlay = SizedArray<Card, max_cards_in_play>;
using CardsPlayed = SizedArray<Card, num_cards_in_hand>;

// Convert strings to cards. Returns true if successful. If unsuccessful, the
// out parameters are untouched.
bool from_string(std::string_view sv, Card& card) noexcept;
bool from_string(std::string_view sv, std::vector<Card>& cards);

// Converts cards to strings.
std::string to_string(Card c);
std::string to_string(const Card* begin, const Card* end);

inline bool operator==(const Card& lhs, const Card& rhs) noexcept
{
   return (lhs.rank() == rhs.rank()) && (lhs.suit() == rhs.suit());
}

inline bool operator!=(const Card& lhs, const Card& rhs) noexcept
{
   return !operator==(lhs, rhs);
}

inline int rank_value(Rank rank) noexcept
{
   return std::min<int>(rank, max_card_value);
}

inline int rank_ordinal(Rank rank) noexcept
{
   return rank - min_card_rank;
}

inline bool rank_is_valid(Rank rank) noexcept
{
   return (rank >= min_card_rank) && (rank <= max_card_rank);
}

inline bool suit_is_valid(Suit suit) noexcept
{
   return (suit >= min_card_suit) && (suit <= max_card_suit);
}

inline constexpr Card::Card() noexcept
: rank_(0), suit_(0)
{ }

constexpr Card nullcard;
// It's convenient to use a null card to announce 'go'.
constexpr Card go_card;

inline Card::Card(int ordinal) noexcept
: rank_((ordinal % num_card_ranks) + min_card_rank),
  suit_((ordinal / num_card_ranks) + min_card_suit)
{
   assert(rank_is_valid(rank_));
   assert(suit_is_valid(suit_));
}

inline Card::Card(Rank rank, Suit suit) noexcept
: rank_(rank), suit_(suit)
{
   assert(rank_is_valid(rank_));
   assert(suit_is_valid(suit_));
}

inline Rank Card::rank() const noexcept
{
   return rank_;
}

inline Suit Card::suit() const noexcept
{
   return suit_;
}

inline int Card::value() const noexcept
{
   return rank_value(rank_);
}

inline bool Card::is_jack() const noexcept
{
   return rank_ == jack;
}

inline bool Card::is_null() const noexcept
{
   return rank_ == 0;
}

#endif /* Card_h */
