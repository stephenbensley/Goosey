//
// Copyright 2022 Stephen E. Bensley
//
// This file is licensed under the MIT License. You may obtain a copy of the
// license at https://github.com/stephenbensley/Goosey/blob/main/LICENSE.
//

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
   if (show_pone()) {
      return true;
   }
   if (show_dealer()) {
      return true;
   }
   if (show_crib()) {
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
   auto [points, game_over] = model_.reveal_starter(deck_.deal_card());
   dispatch_starter_revealed(points);
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
      auto [points, game_over] = model_.play_card(card);
      dispatch_play(player, card, points);
      if (game_over) {
         return true;
      }
   } while (!model_.play_complete());

   return false;
}

bool GameController::show_pone()
{
   auto [points, game_over] = model_.show_pone();
   dispatch_hand_show(model_.pone(), points);
   return game_over;
}

bool GameController::show_dealer()
{
   auto [points, game_over] = model_.show_dealer();
   dispatch_hand_show(model_.dealer(), points);
   return game_over;
}

bool GameController::show_crib()
{
   auto [points, game_over] = model_.show_crib(crib_.data());
   dispatch_crib_show(points);
   return game_over;
}

void GameController::start_new_round()
{
   model_.start_new_round();
   crib_.clear();
}

void GameController::dispatch_starter_revealed(int points) const noexcept
{
   for (auto p : players_) {
      p->on_starter_revealed(model_, model_.starter(), points);
   }
}

void GameController::dispatch_play(PlayerIndex player,
                                   Card card,
                                   int points) const noexcept
{
   for (auto p : players_) {
      p->on_play(model_, { player, card }, points);
   }
}

void GameController::dispatch_hand_show(PlayerIndex player,
                                        int points) const noexcept
{
   for (auto p : players_) {
      p->on_hand_show(model_, player, model_.hand(player), points);
   }
}

void GameController::dispatch_crib_show(int points) const noexcept
{
   for (auto p : players_) {
      p->on_crib_show(model_, model_.crib(), points);
   }
}

void GameController::dispatch_game_over() const noexcept
{
   for (auto p : players_) {
      p->on_game_over(model_, model_.winner());
   }
}
