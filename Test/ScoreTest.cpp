#include "Catch.hpp"
#include "Score.h"
#include "Card.h"
#include <random>

std::random_device rd;
std::mt19937 rng(rd());

TEST_CASE("score_hand", "[score]")
{
   auto [hand_sz, starter_sz, expected_points, crib, type] =
      GENERATE(table<const char*, const char*, int, bool, const char*>({
         {"5H QH 2S 7C", "9D",  2, false, "2-card 15"},
         {"5H XH XS 7C", "9D",  6, false, "2-card double 15 + pair"},
         {"5H XH XS XC", "9D", 12, false, "2-card triple 15 + pair royal"},
         {"5H XH XS XC", "XD", 20, false, "2-card quadruple 15 + double pair royal"},
         {"5H 5S XS XC", "9D", 12, false, "2-card double double 15 + two pairs"},
         {"5H 5H XS XC", "XD", 20, false, "2-card double triple 15 + pair + pair royal"},
         {"2H 3H KS 7C", "9D",  2, false, "3-card 15"},
         {"2H 3H XS XC", "9D",  6, false, "3-card double 15 + pair"},
         {"2H 3H XS XC", "XD", 12, false, "3-card triple 15 + pair royal"},
         {"2H 3H 3S XC", "XD", 12, false, "3-card double double 15 + two pairs"},
         {"AH 2H 4S 9C", "7D",  2, false, "4-card 15"},
         {"AH 2H 4S 9C", "9D",  6, false, "4-card double 15 + pair"},
         {"AH 2H 3S 4C", "5D",  7, false, "5-card 15 + 5-card run"},
         {"7H 8H 5S QC", "4D",  4, false, "2-card 15 + 2-card 15"},
         {"5H XH 7S 7C", "8D",  8, false, "2-card 15 + 2-card double 15 + pair"},
         {"8H 9C XS AD", "2H",  3, false, "3-card run"},
         {"8H 9C XS XD", "2H",  8, false, "3-card double run + pair"},
         {"8H 9C 9S 9D", "XH", 15, false, "3-card triple run + pair royal"},
         {"8H 8C 9S 9D", "XH", 16, false, "3-card double double run + two pairs"},
         {"2H 3C 4S 5D", "7H",  6, false, "4-card run + pair"},
         {"7H 8C 9S 9D", "XH", 12, false, "4-card double run + pair + 15"},
         {"2H 4H 6H 8H", "KD",  4, false, "4-card flush in hand"},
         {"2H 4H 6H 8H", "KH",  5, false, "5-card flush in hand"},
         {"2H 4H 6H 8H", "KD",  0, true,  "4-card flush in crib"},
         {"2H 4H 6H 8H", "KH",  5, true,  "5-card flush in crib"},
         {"2C 4S 6H JH", "KH",  1, false, "his nob"},
         {"2C 4S 6H JH", "KD",  0, false, "jack, but not his nob"}
    }));

   CAPTURE(type);
   CAPTURE(hand_sz, starter_sz, crib);

   std::vector<Card> hand;
   from_string(hand_sz, hand);
   std::shuffle(hand.begin(), hand.end(), rng);
   auto data = hand.data();
   Card starter;
   from_string(starter_sz, starter);

   auto points = score_hand(data, data + hand.size(), starter, crib);
   REQUIRE(points == expected_points);
}

TEST_CASE("score_table", "[score]")
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
   auto data = cards.data();
   
   auto points = score_cards_in_play(data, data + cards.size(), count);
   REQUIRE(points == expected_points);
}
