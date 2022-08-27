#include "DiscardActions.h"
#include "CardSet.h"
#include "CardSplitter.h"
#include <algorithm>
#include <array>
#include <set>

// Computes the unique key for a given hand split.
uint64_t compute_key(const CardSplitter& splitter) noexcept;

DiscardActions::DiscardActions(const CardsDealt& cards) noexcept
{
   // Iterate through all the discard combos.
   CardSplitter splitter(cards);
   // Keep track of which ones we've seen, so we can detect duplicates.
   std::set<uint64_t> seen;

   for (auto i = 0; i < num_discard_actions; ++i) {
      auto [iter, inserted] = seen.insert(compute_key(splitter));
      if (inserted) {
         actions_.set(i);
      }
      splitter.next();
   }
}

// Computes the key for either the hand or the crib in a card split.
uint32_t compute_split_key(const CardSet& all,
                           const Card* begin,
                           const Card* end) noexcept;

// Computes the unique key for a given hand split.
uint64_t compute_key(const CardSplitter& splitter) noexcept
{
   CardSet all(splitter.cards.begin(), splitter.cards.end());
   uint64_t key = compute_split_key(all,
                                    splitter.hand.begin(),
                                    splitter.hand.end());
   key <<= 32;
   key |= compute_split_key(all,
                            splitter.crib.begin(),
                            splitter.crib.end());
   return key;
}

uint32_t compute_split_key(const CardSet& all,
                           const Card* begin,
                           const Card* end) noexcept
{
   uint32_t key = 0;

   // First 16 bits are the sorted ranks.
   static_assert(max_card_rank < 16);
   static_assert(num_cards_in_hand <= 4);
   std::array<int, num_cards_in_hand> ranks{0};
   std::transform(begin, end, ranks.begin(), [](auto c) {
      return c.rank();
   });
   std::sort(ranks.begin(), ranks.end());
   std::for_each(ranks.begin(), ranks.end(), [&key](auto r) {
      key <<= 4;
      key |= r;
   });

   // Next 3 bits are the number of cards of the same suit as any jacks.
   static_assert(num_cards_in_hand < 8);
   key <<= 3;
   int jack_suit_count = 0;
   std::for_each(begin, end, [&](auto c) {
      if (c.is_jack()) {
         jack_suit_count += all.count(c);
      }
   });
   key |= jack_suit_count;

   // Last 13 bits are the rank flags for the flush suit (or zero if no flush).
   static_assert(num_card_ranks <= 13);
   key <<= 13;
   auto i = std::find_if(begin, end, [begin](auto c) {
      return c.suit() != begin->suit();
   });
   if (i == end) {
      key |= all[*begin];
   }

   return key;
}
