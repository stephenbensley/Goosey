#include "Catch.hpp"
#include "CardPlayScore.h"

TEST_CASE("CardPlayScore::play_rank", "[score]")
{
   auto [cards_sz, count, expected_points, title] =
      GENERATE(table<const char*, int, int, const char*>({
         {"AH 8S 3D 2C",    -1,  0, "split run"},
         {"3C 3D 5H",       -1,  0, "pair break run"},
         {"9S AH 3D 2C",    -1,  5, "3-card run + 15 (implicit)"},
         {"9S AH 3D 2C",    15,  5, "3-card run + 15 (explicit)"},
         {"3S 3H 3C",       -1,  6, "pair royal"},
         {"3S 3H 3C 3D",    -1, 12, "double pair royal"},
         {"AS 2H 3H 4H 5D", -1,  7, "5-card run + 15 (implicit"},
      }));

   CAPTURE(title);
   CAPTURE(cards_sz);

   std::vector<Card> cards;
   from_string(cards_sz, cards);
   CardPlayScore score;
   auto points = 0;
   for (auto c : cards) {
      points = score.update_and_score(c.rank());
   }
   REQUIRE(points == expected_points);
}
