#include "Catch.hpp"
#include "CardPlayHands.h"

void test_total_weight(const RankCounts& ranks_seen,
                       int expected_total) noexcept
{
   CardPlayHandsIterator it(4, ranks_seen);
   int total_weight = 0;
   while (it.next()) {
      total_weight += it.combos();
   }
   CHECK(total_weight == expected_total);
}

TEST_CASE("CardPlayHands")
{
   {
      RankCounts ranks_seen;
      // We haven't seen any cards, so number of hands is C(52, 4)
      int expected_total = (52 * 51 * 50 * 49)/(4 * 3 * 2 * 1);
      test_total_weight(ranks_seen, expected_total);
   }
   {
      RankCounts ranks_seen;
      ranks_seen += 4;
      ranks_seen += 4;
      ranks_seen += 5;
      ranks_seen += 9;
      // We've seen four cards, so number of hands is C(48, 4)
      int expected_total = (48 * 47 * 46 * 45)/(4 * 3 * 2 * 1);
      test_total_weight(ranks_seen, expected_total);
   }
}
