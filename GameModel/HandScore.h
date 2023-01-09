//
// Copyright 2022 Stephen E. Bensley
//
// This file is licensed under the MIT License. You may obtain a copy of the
// license at https://github.com/stephenbensley/Goosey/blob/main/LICENSE.
//

#ifndef HandScore_h
#define HandScore_h

#include "Card.h"
#include "RankKeys.h"
#include <algorithm>
#include <array>
#include <bitset>
#include <cstdint>
#include <unordered_map>

// Creates a map of all possible hands to their corresponding point values
// without regard to suit (i.e., no points for flushes or his nob).
class SuitlessScores
{
public:
   using ScoreMap = std::unordered_map<UnorderedRanksKey::KeyType, int>;

   // Creating the map is expensive, so everyone shares a read-only singleton.
   static const ScoreMap& get() noexcept;

private:
   // An extra card for the starter.
   static constexpr int num_cards = num_cards_in_hand + 1;

   // Constructor is private since this is a singleton.
   SuitlessScores();

   // Add the given hand to the ScoreMap.
   void insert(const std::array<Card, num_cards>& cards);

   ScoreMap scores_;
};

// Scores a hand using a cached lookup, rather than starting from scratch.
// Also supports incremental update, which is useful when traversing game trees.
class HandScore
{
public:
   explicit HandScore(bool crib) noexcept;
   HandScore(const Card* begin, const Card* end, bool crib) noexcept;

   void update(Card c) noexcept;
   void update(const Card* begin, const Card* end) noexcept;

   // Returns the number of points.
   int score(Card starter) const noexcept;

private:
   UnorderedRanksKey key_;
   const SuitlessScores::ScoreMap& scores_ = SuitlessScores::get();
   bool crib_;
   // Tracks which jacks are in the hand.
   std::bitset<num_card_suits + 1> jacks_;
   // Tracks the flush suit or set to -1 if no flush.
   int flush_suit_ = -1;
};

inline HandScore::HandScore(bool crib) noexcept
: crib_(crib)
{ }

#endif /* HandScore_h */
