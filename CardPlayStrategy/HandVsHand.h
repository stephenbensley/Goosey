//
// Copyright 2022 Stephen E. Bensley
//
// This file is licensed under the MIT License. You may obtain a copy of the
// license at https://github.com/stephenbensley/Tensity/blob/main/LICENSE.
//

#ifndef HandVsHand_h
#define HandVsHand_h

#include "Card.h"
#include "RankKeys.h"
#include <array>
#include <cstdint>
#include <memory>
#include <unordered_map>

// Maintains a table of results for every possible combination of dealer and
// pone hands.
class HandVsHand
{
public:
   struct Cell {
      uint8_t dealer_points;
      uint8_t pone_points;
   };

   HandVsHand();
   HandVsHand(HandVsHand&) = delete;
   HandVsHand& operator=(HandVsHand&) = delete;

   // 2D table, so [] returns a pointer to a column.
   Cell* operator[](int pos) noexcept;
   const Cell* operator[](int pos) const noexcept;

   // Look up the ordinal in the table for a given hand.
   int ordinal(const CardsKept& hand) const noexcept;
   int ordinal(const RanksInHand& hand) const noexcept;

   // Build the table.
   void build();

   // Load/save the data from/to a file.
   bool load(const char* filename) noexcept;
   void save(const char* filename) const noexcept;

private:
   // Worker function for each thread.
   void build_worker(int idx, int num_workers) noexcept;

   // Play a round between the hands and return the number of points scored by
   // dealer and pone.
   static std::pair<int, int> play_round(const RanksInHand& dealer,
                                         const RanksInHand& pone) noexcept;

   // These assumptions make the combinations calculation easier.
   static_assert(num_card_ranks == 13);
   static_assert(num_cards_in_hand == 4);
   static_assert(num_card_suits >= num_cards_in_hand);

   // Number of k-element combos of n objects with repetition is C(n+k+1, k)
   static constexpr int num_hands = (16 * 15 * 14 * 13)/(4 * 3 * 2 * 1);

   // Table of outcomes. We store it as a one-dimensional array, but
   // conceptually it's a two-dimensional array. The array is dynamically
   // allocated, so that HandVsHand can be allocated on the stack if desired.
   using Table = std::array<Cell, num_hands * num_hands>;
   std::unique_ptr<Table> table_;

   // Index mapping hands to their ordinal in the table.
   using KeyType = UnorderedRanksKey::KeyType;
   std::unordered_map<KeyType, int> index_;

   // Helper function to compute the UnorderedRanksKey for a hand.
   static KeyType key(const CardsKept& hand) noexcept;
   static KeyType key(const RanksInHand& hand) noexcept;
};

inline HandVsHand::Cell* HandVsHand::operator[](int pos) noexcept
{
   return table_->begin() + (num_hands * pos);
}

inline const HandVsHand::Cell* HandVsHand::operator[](int pos) const noexcept
{
   return table_->begin() + (num_hands * pos);
}

inline int HandVsHand::ordinal(const CardsKept& hand) const noexcept
{
   auto i = index_.find(key(hand));
   assert(i != index_.end());
   return i->second;
}

inline int HandVsHand::ordinal(const RanksInHand& hand) const noexcept
{
   auto i = index_.find(key(hand));
   assert(i != index_.end());
   return i->second;
}

#endif /* HandVsHand_h */
