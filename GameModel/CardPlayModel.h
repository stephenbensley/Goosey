#ifndef CardPlay_h
#define CardPlay_h

#include "Card.h"
#include "CardPlayScore.h"
#include "PlayerIndex.h"

// Represents the card play portion of a round of cribbage.
class CardPlayModel
{
public:
   explicit CardPlayModel(PlayerIndex dealer = 0) noexcept;

   PlayerIndex current_player() const noexcept;
   bool is_legal_play(Rank rank) const noexcept;
   // Not the # of cards, but the count of the cards' values.
   int count() const noexcept;
   bool round_over() const noexcept;

   // Plays the card for the current player and returns the number of points
   // scored.
   int play_rank(Rank rank) noexcept;

   // Clear the table and start a new round.
   void start_new_round(PlayerIndex dealer) noexcept;

private:
   // Announces 'go' for the current player and returns the number of points
   // scored.
   int announce_go() noexcept;
   // Advance to the next player.
   void next_player() noexcept;
   // Start a new series (i.e., count goes back to zero).
   void start_new_series() noexcept;

   // Used for scoring card play.
   CardPlayScore score_;
   // Index of player whose turn it is
   char current_;
   // Number of cards played so far in the round.
   char cards_played_ = 0;
   // Count of the current series.
   char count_ = 0;
   // True if one player has announced go.
   bool go_announced_ = false;
   // 'go' tracking would have to be enhanced to support more than two players.
   static_assert(num_players == 2);
};

inline CardPlayModel::CardPlayModel(PlayerIndex dealer) noexcept
: current_(::next_player(dealer))
{ }

inline PlayerIndex CardPlayModel::current_player() const noexcept
{
   return current_;
}

inline bool CardPlayModel::is_legal_play(Rank rank) const noexcept
{
   return (count_ + rank_value(rank)) <= max_count_in_play;
}

inline int CardPlayModel::count() const noexcept
{
   return count_;
}

inline bool CardPlayModel::round_over() const noexcept
{
   return cards_played_ == max_cards_in_play;
}

inline void CardPlayModel::next_player() noexcept
{
   current_ = ::next_player(current_);
}

inline void CardPlayModel::start_new_series() noexcept
{
   score_.clear();
   count_ = 0;
   go_announced_ = false;
}

#endif /* CardPlay_h */
