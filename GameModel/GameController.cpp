#include "GameController.h"
#include "Deck.h"

GameController::GameController(const Players& players,
                               Deck& deck,
                               PlayerIndex first_deal) noexcept
: players_(players),
  deck_(deck),
  model_(first_deal)
{ }

PlayerIndex GameController::play()
{
   while (!play_round())
   { }
   dispatch_game_over();
   return model_.winner();
}

bool GameController::play_round()
{
   deal_cards();
   form_crib();
   if (reveal_starter()) {
      return true;
   }
   if (play_hands()) {
      return true;
   }
   if (model_.show_hands(crib_.data())) {
      return true;
   }
   start_new_round();
   return false;
}

void GameController::deal_cards() noexcept
{
   deck_.shuffle();

   // It's important to deal cards to players in a consistent order relative to
   // the dealer. This ensures that if we use the same rng seed, players will
   // see the same cards regardless of who has first deal.
   for (auto player : deal_order(model_.dealer())) {
      for (auto j = 0; j < num_cards_dealt_per_player; ++j) {
         hands_[player].insert(deck_.deal_card());
      }
   }

   // Some variants of cribbage deal cards directly to the crib.
   for (auto i = 0; i < num_cards_dealt_to_crib; ++i) {
      crib_.push_back(deck_.deal_card());
   }
}

void GameController::form_crib()
{
   for (auto i = 0; i < num_players; ++i) {
      auto discards = players_[i]->get_discards(model_, hands_[i]);
      [[maybe_unused]] auto erased = hands_[i].erase(discards.begin(),
                                                     discards.end());
      assert(erased == discards.size());
      crib_.insert(discards.begin(), discards.end());
   }
}

bool GameController::reveal_starter()
{
   auto game_over = model_.reveal_starter(deck_.deal_card());
   dispatch_starter_revealed();
   return game_over;
}

bool GameController::play_hands()
{
   do {
      auto player = model_.current_player();
      auto card = players_[player]->get_card_to_play(model_, hands_[player]);
      if (card != go_card) {
         [[maybe_unused]] auto erased = hands_[player].erase(card);
         assert(erased);
      }
      auto game_over = model_.play_card(card);
      dispatch_opponent_play(player, card);
      if (game_over) {
         return true;
      }
   } while (!model_.play_complete());

   return false;
}

void GameController::start_new_round()
{
   dispatch_round_over();
   model_.start_new_round();
   crib_.clear();
}

void GameController::dispatch_starter_revealed() const noexcept
{
   for (auto p : players_) {
      p->on_starter_revealed(model_, model_.starter());
   }
}

void GameController::dispatch_opponent_play(PlayerIndex player,
                                            Card card) const noexcept
{
   for (auto p : players_) {
      if (p->index() != player) {
         p->on_opponent_play(model_, { player, card });
      }
   }
}

void GameController::dispatch_round_over() const noexcept
{
   for (auto p : players_) {
      p->on_round_over(model_);
   }
}

void GameController::dispatch_game_over() const noexcept
{
   for (auto p : players_) {
      p->on_game_over(model_, model_.winner());
   }
}
