#include "Catch.hpp"
#include "CardPlayHands.h"

// Iterates through all the hands and verifies the total number of combos seen
// is expected.
void test_total_combos(const RankCounts& ranks_seen, int expected) noexcept
{
   CardPlayHandsIterator it(num_cards_in_hand, ranks_seen);
   auto total = 0;
   while (it.next()) {
      total += it.combos();
   }
   CHECK(total == expected);
}

TEST_CASE("CardPlayHands")
{
   {
      RankCounts ranks_seen;
      // We haven't seen any cards, so number of combos is C(52, 4)
      auto expected = (52 * 51 * 50 * 49)/(4 * 3 * 2 * 1);
      test_total_combos(ranks_seen, expected);
   }
   {
      RankCounts ranks_seen;
      // Pick any four cards.
      ranks_seen += 4;
      ranks_seen += 4;
      ranks_seen += 5;
      ranks_seen += 9;
      // We've seen four cards, so number of hands is C(48, 4)
      auto expected = (48 * 47 * 46 * 45)/(4 * 3 * 2 * 1);
      test_total_combos(ranks_seen, expected);
   }
}
