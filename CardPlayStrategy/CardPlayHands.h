//
// Copyright 2022 Stephen E. Bensley
//
// This file is licensed under the MIT License. You may obtain a copy of the
// license at https://github.com/stephenbensley/Goosey/blob/main/LICENSE.
//

#ifndef CardPlayHands_h
#define CardPlayHands_h

#include "Card.h"
#include <array>
#include <cassert>
#include <vector>

// Maintains a count for each rank. Enables efficient computation of how many
// different combinations exist for a given hand.
class RankCounts
{
public:
   // Returns the number of different ways the hand can be formed given that
   // the cards tracked by 'this' are not available.
   int remaining_combos(const RankCounts& hand) const noexcept;

   // Clear all counts to zero.
   void clear() noexcept;

   // Add all remaining cards of the given rank.
   void add_all(Rank rank) noexcept;

   // Add the rank to the current count.
   RankCounts& operator +=(Rank rhs) noexcept;
   
private:
   std::array<int, max_card_rank + 1> counts_{};
   // Pre-computed values for C'(n, k) = C(num_card_suits - n, k)
   static const int Cprime[5][5];
   // Lookup table assumes we never have more than four cards of a given rank.
   static_assert(num_card_suits <= 4);
};

inline void RankCounts::clear() noexcept
{
   counts_.fill(0);
}

inline void RankCounts::add_all(Rank rank) noexcept
{
   counts_[rank] = num_card_suits;
}

inline RankCounts& RankCounts::operator +=(Rank rhs) noexcept
{
   assert(counts_[rhs] < num_card_suits);
   ++counts_[rhs];
   return *this;
}

// Collection of all possible card play hands.
class CardPlayHands
{
public:
   // State tracked for each hand.
   struct Hand {
      RanksInHand hand;
      RankCounts counts;

      Hand() = default;
      Hand(const Rank* begin, const Rank* end) noexcept;
   };

   // Returns all possible hands made up of the given number of cards.
   const std::vector<Hand>& hands(int num_cards) const noexcept;

   // It's moderately expensive to construct one of these objects, and it's
   // read-only after construction, so make it a singleton.
   static const CardPlayHands& get() noexcept;

private:
   CardPlayHands();

   // Entries for every possible hand.
   std::array<std::vector<Hand>, num_cards_in_hand + 1> hands_;
};

inline const std::vector<CardPlayHands::Hand>&
CardPlayHands::hands(int num_cards) const noexcept
{
   assert(num_cards >= 0);
   assert(num_cards <= num_cards_in_hand);
   return hands_[num_cards];
}

// Emumerates card play hands.
class CardPlayHandsIterator
{
public:
   // After contructing the iterator, you must call next() to load the first
   // hand.
   CardPlayHandsIterator(int num_cards, const RankCounts& ranks_seen);

   // Advance to the next hand. Returns false if there are no more combos.
   bool next() noexcept;
   // Returns the position of the iterator.
   int pos() const noexcept;

   // Hand for current iterator position.
   const RanksInHand& hand() const noexcept;
   // Number of combos for hand from ranks_seen's perspective.
   int combos() const noexcept;
   // Number of combos for ranks_seen from hand's perspective.
   int rcombos() const noexcept;

private:
   const std::vector<CardPlayHands::Hand>& hands_;
   RankCounts ranks_seen_;
   int pos_ = -1;
   int combos_ = 0;
};

inline int CardPlayHandsIterator::pos() const noexcept
{
   return pos_;
}

inline const RanksInHand& CardPlayHandsIterator::hand() const noexcept
{
   assert(pos_ >= 0);
   assert(pos_ < hands_.size());
   return hands_[pos_].hand;
}

inline int CardPlayHandsIterator::combos() const noexcept
{
   assert(pos_ >= 0);
   assert(pos_ < hands_.size());
   assert(combos_ > 0);
   return combos_;
}

inline int CardPlayHandsIterator::rcombos() const noexcept
{
   return hands_[pos_].counts.remaining_combos(ranks_seen_);
}

#endif /* CardPlayHands_h */
