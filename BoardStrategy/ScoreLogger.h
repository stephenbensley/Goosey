#ifndef ScoreLogger_h
#define ScoreLogger_h

#include "Player.h"
#include "ScoreLog.h"

// Wraps a player instance and logs all the points scored.
class ScoreLogger : public Player
{
public:
   ScoreLogger(const Player& observed,
               std::shared_ptr<ScoreLog> log = std::make_shared<ScoreLog>());

   ScoreLog& log() noexcept;

   virtual std::unique_ptr<Player> clone() const override;

   virtual CardsDiscarded get_discards(const GameView& game,
                                       const CardsInHand& hand) override;
   virtual Card get_card_to_play(const GameView& game,
                                 const CardsInHand& hand) override;

   virtual void on_set_index(PlayerIndex new_value) noexcept override;

   virtual void on_starter_revealed(const GameView& game,
                                    Card starter,
                                    int points) override;
   virtual void on_play(const GameView& game, Play play, int points) override;
   virtual void on_hand_show(const GameView& game,
                             PlayerIndex player,
                             const CardsPlayed& hand,
                             int points) override;
   virtual void on_crib_show(const GameView& game,
                             const CardsInCrib& crib,
                             int points) override;
   virtual void on_game_over(const GameView& game, PlayerIndex winner) override;

private:
   std::unique_ptr<Player> observed_;
   std::shared_ptr<ScoreLog> log_;
   ScoreRecord record_;
};

inline ScoreLog& ScoreLogger::log() noexcept
{
   return *log_;
}

#endif /* ScoreLogger_h */
