//
// Copyright 2022 Stephen E. Bensley
//
// This file is licensed under the MIT License. You may obtain a copy of the
// license at https://github.com/stephenbensley/Tensity/blob/main/LICENSE.
//

#ifndef Player_h
#define Player_h

#include "GameView.h"

// Abstract interface implemented by a cribbage player.
class Player
{
public:
   virtual ~Player() = default;

   // Set the index for this player. May only be called once.
   void set_index(PlayerIndex new_value);

   // Create a clone of this player. When conducting multi-threaded matches, a
   // clone is used for each thread, so that instances don't have to be
   // thread-safe.
   virtual std::unique_ptr<Player> clone() const = 0;

   // Helper functions to query game state.
   PlayerIndex index() const noexcept;
   bool is_dealer(const GameView& game) const noexcept;
   static bool is_legal_play(const GameView& game, Card card) noexcept;

   // Must be implemented by the player to return their move.
   virtual CardsDiscarded get_discards(const GameView& game,
                                       const CardsInHand& hand) = 0;
   virtual Card get_card_to_play(const GameView& game,
                                 const CardsInHand& hand) = 0;

   // Notifies the player of interesting game events. A Player does not have
   // to implement these if they don't need the notification.
   virtual void on_set_index(PlayerIndex new_value);
   virtual void on_starter_revealed(const GameView& game,
                                    Card starter,
                                    int points);
   virtual void on_play(const GameView& game, Play play, int points);
   virtual void on_hand_show(const GameView& game,
                             PlayerIndex player,
                             const CardsPlayed& hand,
                             int points);
   virtual void on_crib_show(const GameView& game,
                             const CardsInCrib& crib,
                             int points);
   virtual void on_game_over(const GameView& game, PlayerIndex winner);

protected:
   Player() = default;
   Player(const Player&) = default;
   Player& operator=(const Player&) = default;

private:
   PlayerIndex index_ = invalid_player;
};

using Players = std::array<Player*, num_players>;

inline PlayerIndex Player::index() const noexcept
{
   return index_;
}

inline bool Player::is_dealer(const GameView& game) const noexcept
{
   return game.dealer() == index();
}

inline bool Player::is_legal_play(const GameView& game, Card card) noexcept
{
   return game.count() + card.value() <= max_count_in_play;
}

#endif /* Player_h */
