#ifndef GameModel_h
#define GameModel_h

#include "CardPlayModel.h"
#include "GameView.h"

// Represents the state of a cribbage game, enforces the FSM that governs
// play, and tallies the score.
class GameModel : public GameView
{
public:
   explicit GameModel(PlayerIndex first_deal) noexcept;

   PlayerIndex current_player() const noexcept;
   bool play_complete() const noexcept;

   // These methods return 'true' if the game is over.
   [[nodiscard]] bool reveal_starter(Card card) noexcept;
   [[nodiscard]] bool play_card(Card card) noexcept;
   [[nodiscard]] bool show_hands(const CardsShown& crib) noexcept;

   void start_new_round() noexcept;

 private:
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

#endif /* GameModel_h */
