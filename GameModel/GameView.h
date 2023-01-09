//
// Copyright 2022 Stephen E. Bensley
//
// This file is licensed under the MIT License. You may obtain a copy of the
// license at https://github.com/stephenbensley/Goosey/blob/main/LICENSE.
//

#ifndef GameView_h
#define GameView_h

#include "Card.h"
#include "PlayerIndex.h"
#include "SizedArray.h"
#include <array>
#include <cassert>

// Used to log a play and who made it.
struct Play {
   PlayerIndex player;
   Card card;
};

using Plays = SizedArray<Play, max_cards_in_play>;

// View into the game state -- this is what players see.
class GameView
{
public:
   int score(PlayerIndex player) const noexcept;
   PlayerIndex dealer() const noexcept;
   PlayerIndex pone() const noexcept;
   Card starter() const noexcept;

   // Log of all the plays made so far in the current round.
   const Plays& plays() const noexcept;

   // Iterate through the current series.
   const Play* series_begin() const noexcept;
   const Play* series_end() const noexcept;

   // Running count of the current series.
   int count() const noexcept;

   // Cards played so far in the round. When the round is over, these will be
   // the cards shown for each player.
   const CardsPlayed& hand(PlayerIndex player) const noexcept;

   // Cards discarded to the crib. Empty until the crib is shown.
   const CardsInCrib& crib() const noexcept;

   // At the end of a round, returns the winner of the round; otherwise,
   // returns invalid_player.
   PlayerIndex winner() const noexcept;

protected:
   explicit GameView(PlayerIndex first_deal) noexcept;
   GameView(const GameView&) = default;
   GameView& operator=(const GameView&) = default;
   ~GameView() = default;

   // Protected interface used by the model to update the view.

   // Returns true if the game is over.
   [[nodiscard]] bool peg_points(PlayerIndex player, int points) noexcept;
   void set_starter(Card new_starter) noexcept;
   void play_card(Play play) noexcept;
   void set_crib(const CardsShown& new_crib) noexcept;
   void start_new_series() noexcept;
   void start_new_round() noexcept;

private:
   std::array<int, num_players> scores_{0};
   PlayerIndex dealer_;
   Card starter_ = nullcard;
   Plays plays_;
   // Offset into plays_ where the current series begins.
   int series_offset_ = 0;
   int count_ = 0;
   std::array<CardsPlayed, num_players> hands_;
   CardsInCrib crib_;
   PlayerIndex winner_ = invalid_player;
};

inline int GameView::score(PlayerIndex player) const noexcept
{
   assert(is_valid_player(player));
   return scores_[player];
}

inline PlayerIndex GameView::dealer() const noexcept
{
   return dealer_;
}

inline PlayerIndex GameView::pone() const noexcept
{
   return other_player(dealer_);
}

inline Card GameView::starter() const noexcept
{
   return starter_;
}

inline const Plays& GameView::plays() const noexcept
{
   return plays_;
}

inline const Play* GameView::series_begin() const noexcept
{
   return plays_.begin() + series_offset_;
}

inline const Play* GameView::series_end() const noexcept
{
   return plays_.end();
}

inline int GameView::count() const noexcept
{
   return count_;
}

inline const CardsPlayed& GameView::hand(PlayerIndex player) const noexcept
{
   assert(is_valid_player(player));
   return hands_[player];
}

inline const CardsInCrib& GameView::crib() const noexcept
{
   return crib_;
}

inline PlayerIndex GameView::winner() const noexcept
{
   return winner_;
}

#endif /* GameView_h */
