//
// Copyright 2022 Stephen E. Bensley
//
// This file is licensed under the MIT License. You may obtain a copy of the
// license at https://github.com/stephenbensley/Goosey/blob/main/LICENSE.
//

#include "Score.h"
#include "Card.h"
#include "SizedArray.h"
#include <algorithm>
#include <array>
#include <bitset>
#include <cassert>
#include <iterator>

// Looks for combinations adding to (15 - subtotal) and returns the number of
// points scored for any combos found.
int score_fifteens_in_hand(const Card* c,
                           const Card* end,
                           int subtotal = 0) noexcept
{
   auto points = 0;
   for ( ; c != end; ++c) {
      auto new_subtotal = subtotal + c->value();
      if (new_subtotal == 15) {
         points += num_points_for_15;
      }  else if (new_subtotal < 15) {
         // We still haven't reached 15, so search the remaining cards
         // recursively.
         points += score_fifteens_in_hand(std::next(c), end, new_subtotal);
      }
      // If neither condition is met, we've exceeded 15 without finding a
      // scoring combo, so nothing to do.
   }
   return points;
}

// Looks for combinations that are runs or pairs and returns the number of
// points scored for any combos found.
int score_runs_and_pairs_in_hand(const Card* begin,
                                 const Card* end) noexcept
{
   // Count the number of cards of each rank. We allocate two extra slots: the
   // first because ranks are 1-based and the second because this guarantees
   // every run will be zero-terminated.
   std::array<int, max_card_rank + 2> counts{};
   std::for_each(begin,
                 end,
                 [&counts](auto c){ ++counts[c.rank()]; }
                 );

   auto points = 0;
   // Length in cards of the current run.
   auto run_length = 0;
   // Point multiplier for the current run due to duplicate ranks.
   auto run_multiplier = 1;
   for (auto c : counts) {
      if (c > 0) {
         // Score points for two or more cards of a given rank.
         points += c * (c - 1);
         // Run length keeps increasing until we hit a zero count.
         ++run_length;
         // Multiple cards of the same rank multiply the scoring of a run.
         run_multiplier *= c;
      } else {
         // We hit an empty count, which terminates the current run. If the run
         // length is >= 3, then score points.
         if (run_length >= 3) {
              points += run_length * run_multiplier;
         }
         // Reset our state, so we can look for another run.
         run_length = 0;
         run_multiplier = 1;
      }
   }
   return points;
}

// Checks for a flush and returns the number of points scored.
int score_flush_in_hand(const Card* begin,
                        const Card* end,
                        Card starter,
                        bool crib) noexcept
{
   // We don't allow partial hands to score points for a flush.
   const auto num_cards_needed = crib ? num_cards_in_crib : num_cards_in_hand;
   if (std::distance(begin, end) != num_cards_needed) {
      return 0;
   }

   // If all cards don't match suit of first card, then it's not a flush.
   auto c = std::find_if(begin,
                         end,
                         [begin](auto c) {
                            return c.suit() != begin->suit();
                         });
   if (c != end) {
      return 0;
   }

   // We know the flush length is at least num_cards_needed since we checked
   // that above. Now, see if the starter card matches as well.
   auto flush_len = num_cards_needed;
   if (begin->suit() == starter.suit()) {
      ++flush_len;
   }

   // Crib must have a flush in the crib AND match the starter.
   const auto min_flush_len = crib ? (num_cards_needed + 1) : num_cards_needed;

   // A valid flush scores as many points as there are cards in the flush.
   return (flush_len < min_flush_len) ? 0 : flush_len;
}

// Checks for his nob and returns the number of points scored.
int score_his_nob_in_hand(const Card* begin,
                          const Card* end,
                          Card starter) noexcept
{
   // Look for a jack of the same suit as the starter.
   auto c = std::find_if(begin,
                         end,
                         [starter](auto c) {
                            return c.is_jack() && (c.suit() == starter.suit());
                         });
   return (c != end) ? num_points_for_his_nob : 0;
}

int score_hand(const Card* begin,
               const Card* end,
               Card starter,
               bool crib) noexcept
{
   assert(std::distance(begin, end) <= num_cards_in_show);

   // Concatenate the cards and the starter.
   SizedArray<Card, num_cards_in_show + 1> cards;
   std::for_each(begin, end, [&cards](auto c){ cards.push_back(c); });
   
    if (!starter.is_null()) {
      cards.push_back(starter);
    }

   return score_fifteens_in_hand(cards.begin(), cards.end()) +
          score_runs_and_pairs_in_hand(cards.begin(), cards.end()) +
          score_flush_in_hand(begin, end, starter, crib) +
          score_his_nob_in_hand(begin, end, starter);

}

int score_cards_in_play(const Card* begin,
                        const Card* end,
                        int count) noexcept
{
   // You can't score points with fewer than two cards. Bailing early simplifies
   // some the following code.
   if (std::distance(begin, end) < 2) {
      return 0;
   }

   if (count < 0) {
      count = 0;
      std::for_each(begin, end, [&count](auto c){ count += c.value(); });
   }

   std::reverse_iterator<const Card*> rbegin(end), rend(begin);
   // Rank of the card on top.
   const auto top_rank = rbegin->rank();

   // Look for pairs.
   auto c = std::find_if(std::next(rbegin),
                         rend,
                         [top_rank](auto c){
                            return c.rank() != top_rank;
                         });
   int match_len = static_cast<int>(std::distance(rbegin, c));
   auto points = match_len * (match_len - 1);

   // Track which ranks we've seen.
   std::bitset<max_card_rank + 1> ranks_seen;
   ranks_seen.set(top_rank);

   // Lowest and highest rank we've seen.
   auto min_rank = top_rank;
   auto max_rank = top_rank;

   // Number of cards processed.
   auto len = 1;
   // Longest run found so far.
   auto run_len = 0;

   for (auto c = std::next(rbegin); c != rend; ++c) {
      // A run can't contain two cards of the same rank, so if we find a dup,
      // we're done.
      if (ranks_seen.test(c->rank())) {
         break;
      }
      ranks_seen.set(c->rank());

      // Update our stats.
      min_rank = std::min(min_rank, c->rank());
      max_rank = std::max(max_rank, c->rank());
      ++len;

      // Do we have a run? If so, track it, but keep looking since we might
      // find a longer one.
      if (len == (max_rank - min_rank + 1)) {
         run_len = len;
      }
   }

   // Run must be at least 3 cards long to count.
   if (run_len >= 3) {
      // Score as many points as there were cards.
      points += run_len;
   }

   if (count == 15) {
      points += num_points_for_15;
   } else if (count == 31) {
      points += num_points_for_31;
   }

   return points;
}
