//
// Copyright 2022 Stephen E. Bensley
//
// This file is licensed under the MIT License. You may obtain a copy of the
// license at https://github.com/stephenbensley/Goosey/blob/main/LICENSE.
//

#include "CardPlayHands.h"

int RankCounts::remaining_combos(const RankCounts& hand) const noexcept
{
   int result = 1;
   for (auto i = min_card_rank; i <= max_card_rank; ++i) {
      result *= Cprime[counts_[i]][hand.counts_[i]];
   }
   return result;
}

const int RankCounts::Cprime[5][5] =
{
   { 1, 4, 6, 4, 1 },
   { 1, 3, 3, 1, 0 },
   { 1, 2, 1, 0, 0 },
   { 1, 1, 0, 0, 0 },
   { 1, 0, 0, 0, 0 }
};

CardPlayHands::Hand::Hand(const Rank* begin, const Rank* end) noexcept
{
   for (auto rank = begin; rank != end; ++rank) {
      hand.push_back(*rank);
      counts += *rank;
   }
}

const CardPlayHands& CardPlayHands::get() noexcept
{
   static CardPlayHands hands;
   return hands;
}

CardPlayHands::CardPlayHands()
{
   // Add an empty entry for the case where no cards are left in the hand.
   hands_[0].push_back(Hand());

   // It's way easier to hardcode this for a standard Cribbage game than to
   // handle all possibilities.
   static_assert(num_cards_in_hand == 4);
   static_assert(num_card_suits >= 4);

   std::array<Rank, num_cards_in_hand> ranks;
   for (auto i0 = min_card_rank; i0 <= max_card_rank; ++i0) {
      ranks[0] = i0;
      hands_[1].emplace_back(ranks.begin(), ranks.begin() + 1);
      for (auto i1 = min_card_rank; i1 <= i0; ++i1) {
         ranks[1] = i1;
         hands_[2].emplace_back(ranks.begin(), ranks.begin() + 2);
         for (auto i2 = min_card_rank; i2 <= i1; ++i2) {
            ranks[2] = i2;
            hands_[3].emplace_back(ranks.begin(), ranks.begin() + 3);
            for (auto i3 = min_card_rank; i3 <= i2; ++i3) {
               ranks[3] = i3;
               hands_[4].emplace_back(ranks.begin(), ranks.begin() + 4);
            }
         }
      }
   }
}

CardPlayHandsIterator::CardPlayHandsIterator(int num_cards,
                                             const RankCounts& ranks_seen)
: hands_(CardPlayHands::get().hands(num_cards)),
  ranks_seen_(ranks_seen)
{

}

bool CardPlayHandsIterator::next() noexcept
{
   do {
      if (++pos_ >= hands_.size()) {
         return false;
      }
      combos_ = ranks_seen_.remaining_combos(hands_[pos_].counts);
   // Skip over hands that are impossible.
   } while (combos_ == 0);

   return true;
}
