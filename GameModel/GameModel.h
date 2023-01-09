//
// Copyright 2022 Stephen E. Bensley
//
// This file is licensed under the MIT License. You may obtain a copy of the
// license at https://github.com/stephenbensley/Goosey/blob/main/LICENSE.
//

#ifndef GameModel_h
#define GameModel_h

#include "CardPlayModel.h"
#include "GameView.h"

// Represents the state of a cribbage game, enforces the FSM that governs
// play, and tallies the score.
class GameModel : public GameView
{
public:
   // Used to return the result of an action take on the model.
   struct Result {
      int points;
      bool game_over;
   };

   explicit GameModel(PlayerIndex first_deal) noexcept;

   PlayerIndex current_player() const noexcept;
   bool play_complete() const noexcept;

   [[nodiscard]] Result reveal_starter(Card card) noexcept;
   [[nodiscard]] Result play_card(Card card) noexcept;
   [[nodiscard]] Result show_pone() noexcept;
   [[nodiscard]] Result show_dealer() noexcept;
   [[nodiscard]] Result show_crib(const CardsShown& crib) noexcept;

   void start_new_round() noexcept;

 private:
   [[nodiscard]] Result get_result(PlayerIndex player, int points) noexcept;
   int score_hand(PlayerIndex player) const noexcept;
   int score_crib() const noexcept;

   CardPlayModel card_play_;
};

inline PlayerIndex GameModel::current_player() const noexcept
{
   return card_play_.current_player();
}

inline bool GameModel::play_complete() const noexcept
{
   return card_play_.round_over();
}

inline GameModel::Result GameModel::get_result(PlayerIndex player,
                                               int points) noexcept
{
   return { points, peg_points(player, points) };
}

#endif /* GameModel_h */
