#include "Catch.hpp"
#include "Deck.h"

TEST_CASE("Deck::shuffle", "[deck]")
{
   Deck deck;

   const int num_rounds = 1000;
   std::array<int, max_card_rank + 1> by_rank{};
   std::array<int, num_card_suits + 1> by_suit{};

   for (auto i = 0; i < num_rounds; ++i) {
      deck.shuffle();
      for (auto j = 0; j < num_cards_dealt_per_round; ++j)
      {
         auto c = deck.deal_card();
         ++by_rank[c.rank()];
         ++by_suit[c.suit()];
      }
   }

   const int num_cards_dealt = num_rounds * num_cards_dealt_per_round;
   const int expected_per_rank = num_cards_dealt / num_card_ranks;
   const int expected_per_suit = num_cards_dealt / num_card_suits;
   const int rank_interval = expected_per_rank / 10;
   const int suit_interval = expected_per_suit / 10;

   for (auto i = min_card_rank; i <= max_card_rank; ++i) {
      REQUIRE(by_rank[i] > expected_per_rank - rank_interval);
      REQUIRE(by_rank[i] < expected_per_rank + rank_interval);
   }

   for (auto i = 1; i <= num_card_suits; ++i) {
      REQUIRE(by_suit[i] > expected_per_suit - suit_interval);
      REQUIRE(by_suit[i] < expected_per_suit + suit_interval);
   }
}
