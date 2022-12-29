#include "Catch.hpp"
#include "Deck.h"

TEST_CASE("Deck::shuffle", "[deck]")
{
   Deck deck;

   // Collect statistics for a large number of hands of cribbage.
   const auto num_hands = 1000;
   std::array<int, max_card_rank + 1> by_rank{};
   std::array<int, num_card_suits + 1> by_suit{};

   for (auto i = 0; i < num_hands; ++i) {
      deck.shuffle();
      for (auto j = 0; j < num_cards_dealt_per_round; ++j)
      {
         auto c = deck.deal_card();
         ++by_rank[c.rank()];
         ++by_suit[c.suit()];
      }
   }

   const auto num_cards_dealt = num_hands * num_cards_dealt_per_round;
   const auto expected_per_rank = num_cards_dealt / num_card_ranks;
   const auto expected_per_suit = num_cards_dealt / num_card_suits;
   
   // Make sure the actual counts are within +/- 10% of the expected counts.
   const auto rank_interval = expected_per_rank / 10;
   const auto suit_interval = expected_per_suit / 10;

   for (auto i = min_card_rank; i <= max_card_rank; ++i) {
      REQUIRE(by_rank[i] > expected_per_rank - rank_interval);
      REQUIRE(by_rank[i] < expected_per_rank + rank_interval);
   }

   for (auto i = 1; i <= num_card_suits; ++i) {
      REQUIRE(by_suit[i] > expected_per_suit - suit_interval);
      REQUIRE(by_suit[i] < expected_per_suit + suit_interval);
   }
}
