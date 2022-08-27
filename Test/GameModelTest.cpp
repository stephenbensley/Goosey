#include "Catch.hpp"
#include "GameModel.h"

struct TestPlay
{
   PlayerIndex player;
   const char* card_sz;
   int count;
   int score;
};

TEST_CASE("GameModel")
{
   Card starter;
   from_string("5H", starter);
   std::vector<Card> crib_vec;
   from_string("XS 8D KC 8C", crib_vec);
   CardsShown crib;
   std::copy(crib_vec.begin(), crib_vec.end(), crib.begin());

   const TestPlay plays[] = {
      {1, "JH", 10, 0},
      {0, "5S", 15, 2},
      {1, "7C", 22, 0},
      {0, "6H", 28, 5},
      {1, "00", 28, 0},
      {0, "2S", 30, 5},
      {0, "00",  0, 6},
      {1, "6D", 06, 0},
      {0, "4S", 10, 6},
      {1, "4H", 14, 3}
   };
   const int num_plays = sizeof(plays)/sizeof(plays[0]);

   GameModel model(0);

   bool game_over = model.reveal_starter(starter);
   REQUIRE(!game_over);

   for (auto i = 0; i < num_plays; ++i) {
      auto player = plays[i].player;
      REQUIRE(model.current_player() == player);

      Card card;
      from_string(plays[i].card_sz, card);
      game_over = model.play_card(card);
      REQUIRE(!game_over);
      REQUIRE(model.count() == plays[i].count);
      REQUIRE(model.score(plays[i].player) == plays[i].score);
   }

   REQUIRE(model.play_complete());

   game_over = model.show_hands(crib);
   REQUIRE(!game_over);

   REQUIRE(model.score(0) == 24);
   REQUIRE(model.score(1) == 12);
}


