//
// Copyright 2022 Stephen E. Bensley
//
// This file is licensed under the MIT License. You may obtain a copy of the
// license at https://github.com/stephenbensley/Goosey/blob/main/LICENSE.
//

#include "CardSplitter.h"
#include <algorithm>

const CardCombos& CardCombos::get() noexcept
{
   static CardCombos combos;
   return combos;
}

CardCombos::CardCombos() noexcept
{
   // Algorithm from Prof. Nathan Wodarz

   // Start by generating the crib combos.
   std::array<int, num_cards_discarded_per_player> combo;
   std::generate(combo.begin(),
                 combo.end(),
                 [i = 0]() mutable { return i++; });

   // Store first combo.
   std::copy(combo.begin(), combo.end(), crib[0]);

   for (auto i = 1; i < num_discard_actions; ++i) {

      // Find the rightmost element not at its maximum value
      auto m = combo.end();
      auto max_val = num_cards_dealt_per_player;
      while (*(--m) == --max_val) { }

      // Don't change anything before that element
      // Increment the element found above
      ++(*m);

      // Each additional element is one more than previous
      while (++m != combo.end()) {
         *m = *(m - 1) + 1;
      }

      // Store next combo.
      std::copy(combo.begin(), combo.end(), crib[i]);
   }

   // Now fill in the hand combos.
   for (auto i = 0; i < num_discard_actions; ++i) {
      const auto begin = crib[i];
      const auto end = begin + num_cards_discarded_per_player;
      auto j = 0;
      for (auto k = 0; k < num_cards_dealt_per_player; ++k) {
         // If it's not in the crib combo, add it to the hand.
         if (std::find(begin, end, k) == end) {
            hand[i][j++] = k;
         }
      }
      assert(j == num_cards_in_hand);
   }
}

CardSplitter::CardSplitter(const CardsDealt& cards) noexcept
: cards(cards)
{
   load();
}

bool CardSplitter::next() noexcept
{
   assert(pos_ < num_discard_actions);
   if (++pos_ < num_discard_actions) {
      load();
      return true;
   }
   return false;
}

void CardSplitter::load() noexcept
{
   for (auto i = 0; i < hand.size(); ++i) {
      hand[i] = cards[combos_.hand[pos_][i]];
   }
   for (auto i = 0; i < crib.size(); ++i) {
      crib[i] = cards[combos_.crib[pos_][i]];
   }
}
