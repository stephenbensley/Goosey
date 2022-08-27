#include "Catch.hpp"
#include "GameModel.h"

struct TestPlay
{
   PlayerIndex player;
   const char* card_sz;
   int count;
   int points;
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
      {1, "JH", 10, 0, 0},
      {0, "5S", 15, 2, 2},
      {1, "7C", 22, 0, 0},
      {0, "6H", 28, 3, 5},
      {1, "00", 28, 0, 0},
      {0, "2S", 30, 0, 5},
      {0, "00",  0, 1, 6},
      {1, "6D", 06, 0, 0},
      {0, "4S", 10, 0, 6},
      {1, "4H", 14, 3, 3}
   };
   const int num_plays = sizeof(plays)/sizeof(plays[0]);

   GameModel model(0);

   auto result = model.reveal_starter(starter);
   REQUIRE(result.points == 0);
   REQUIRE(!result.game_over);

   for (auto i = 0; i < num_plays; ++i) {
      auto player = plays[i].player;
      REQUIRE(model.current_player() == player);

      Card card;
      from_string(plays[i].card_sz, card);
      result = model.play_card(card);
      REQUIRE(result.points == plays[i].points);
      REQUIRE(!result.game_over);
      REQUIRE(model.count() == plays[i].count);
      REQUIRE(model.score(plays[i].player) == plays[i].score);
   }

   REQUIRE(model.play_complete());

   result = model.show_pone();
   REQUIRE(!result.game_over);
   REQUIRE(result.points == 9);
   REQUIRE(model.score(1) == 12);

   result = model.show_dealer();
   REQUIRE(!result.game_over);
   REQUIRE(result.points == 12);
   REQUIRE(model.score(0) == 18);

   result = model.show_crib(crib);
   REQUIRE(!result.game_over);
   REQUIRE(result.points == 6);
   REQUIRE(model.score(0) == 24);
}


