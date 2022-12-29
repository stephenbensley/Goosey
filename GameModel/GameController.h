//
// Copyright 2022 Stephen E. Bensley
//
// This file is licensed under the MIT License. You may obtain a copy of the
// license at https://github.com/stephenbensley/Tensity/blob/main/LICENSE.
//

#ifndef GameController_h
#define GameController_h

#include "GameModel.h"
#include "Player.h"
#include <array>

class Deck;

// Conducts a cribbage game between two Player instances.
class GameController
{
public:
   GameController(const Players& players,
                  Deck& deck,
                  PlayerIndex first_deal) noexcept;

   // Plays a single game of cribbage and returns the winner.
   PlayerIndex play();

private:
   // Returns true if the game is over.
   [[nodiscard]] bool play_round();
   // Deal cards to players (and maybe the crib).
   void deal_cards() noexcept;
   // Ask players which cards they want to lay away and discard these to crib.
   void form_crib();
   // Turn up the starter card and announce it to the players.
   [[nodiscard]] bool reveal_starter();
   // Handles the card play portion of a round. Returns true if the game is
   // over.
   [[nodiscard]] bool play_hands();
   // Score the hands.
   [[nodiscard]] bool show_pone();
   [[nodiscard]] bool show_dealer();
   [[nodiscard]] bool show_crib();
   // Start a new round of play.
   void start_new_round();

   // Dispatch notifications to players.
   void dispatch_starter_revealed(int points) const noexcept;
   void dispatch_play(PlayerIndex player,
                      Card card,
                      int points) const noexcept;
   void dispatch_hand_show(PlayerIndex player, int points) const noexcept;
   void dispatch_crib_show(int points) const noexcept;
   void dispatch_game_over() const noexcept;

   Players players_;
   Deck& deck_;
   std::array<CardsInHand, num_players> hands_;
   CardsInCrib crib_;
   GameModel model_;
};

#endif /* GameController_h */
