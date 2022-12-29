//
// Copyright 2022 Stephen E. Bensley
//
// This file is licensed under the MIT License. You may obtain a copy of the
// license at https://github.com/stephenbensley/Tensity/blob/main/LICENSE.
//

#include "HandScore.h"
#include "Score.h"


const SuitlessScores::ScoreMap& SuitlessScores::get() noexcept
{
   static SuitlessScores scores;
   return scores.scores_;

}

SuitlessScores::SuitlessScores()
{
   // It's much easier to hardcode the nested for-loops than to do something
   // clever that can handle all possibilities.
   static_assert(num_cards == 5);
   static_assert(min_card_suit == 1);
   static_assert(max_card_suit == 4);
   static_assert(min_card_rank == 1);
   static_assert(max_card_rank == 13);

   std::array<Card, num_cards> cards;

   // Suits are carefully chosen to avoid flushes and his nob.
   for (Rank i0 = 1; i0 <= 13; ++i0) {
      cards[0] = { i0, 1 };
      for (Rank i1 = i0; i1 <= 13; ++i1) {
         cards[1] = { i1, 1 };
         for (Rank i2 = i1; i2 <= 13; ++i2) {
            cards[2] = { i2, 2 };
            for (Rank i3 = i2; i3 <= 13; ++i3) {
               cards[3] = { i3, 2 };
               // Don't allow five-of-a-kind.
               for (Rank i4 = std::max<Rank>(i3, i0 + 1); i4 <= 13; ++i4) {
                  cards[4] = { i4, 3 };
                  insert(cards);
               }
            }
         }
      }
   }
}

inline void SuitlessScores::insert(const std::array<Card, num_cards>& cards)
{
   // Crib only matters for flushes, so we can default crib to false.
   auto points = score_hand(cards.begin(),
                            std::prev(cards.end()),
                            cards.back(),
                            false);

   UnorderedRanksKey key;
   std::for_each(cards.begin(), cards.end(), [&key](auto& card) {
      key.insert(card.rank());
   });

   scores_[key()] = points;
}

HandScore::HandScore(const Card* begin, const Card* end, bool crib) noexcept
: crib_(crib)
{
   update(begin, end);
}

void HandScore::update(Card c) noexcept
{
   if (c.is_jack()) {
      jacks_.set(c.suit());
   }

   if (key_.empty()) {
      // First card we've seen, so this is our candidate for flush suit.
      flush_suit_ = c.suit();
   } else if (flush_suit_ != c.suit()) {
      // Any mismatch means no flush possible.
      flush_suit_ = -1;
   }

   key_.insert(c.rank());
}

void HandScore::update(const Card* begin, const Card* end) noexcept
{
   std::for_each(begin, end, [this](auto c){ update(c); });
}

int HandScore::score(Card starter) const noexcept
{
   // Create a copy, so we can modify it. This allows score to be called
   // multiple times with different starters.
   UnorderedRanksKey key(key_);
   key.insert(starter.rank());

   auto i = scores_.find(key());
   // Map contains every possible hand, so lookup can never fail.
   assert(i != scores_.end());
   auto points = i->second;

   // Now fix up the scoring that depends on suit.

   if (jacks_.test(starter.suit())) {
      points += num_points_for_his_nob;
   }

   if (flush_suit_ != -1) {
       if (starter.suit() == flush_suit_) {
         points += (num_cards_in_hand + 1);
      } else if (!crib_) {
         points += num_cards_in_hand;
      }
   }

   return points;
}
