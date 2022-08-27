#include "ScoreLogger.h"

ScoreLogger::ScoreLogger(const Player& observed,
                         std::shared_ptr<ScoreLog> log)
: observed_(observed.clone()),
  log_(log)
{
}

std::unique_ptr<Player> ScoreLogger::clone() const
{
   return std::make_unique<ScoreLogger>(*observed_, log_);
}

CardsDiscarded ScoreLogger::get_discards(const GameView& game,
                                         const CardsInHand& hand)
{
   assert(record_.empty());
   return observed_->get_discards(game, hand);
}

Card ScoreLogger::get_card_to_play(const GameView& game,
                                   const CardsInHand& hand)
{
   return observed_->get_card_to_play(game, hand);
}

void ScoreLogger::on_set_index(PlayerIndex new_value) noexcept
{
   observed_->set_index(new_value);
}

void ScoreLogger::on_starter_revealed(const GameView& game,
                                      Card starter,
                                      int points)
{
   // Starter points always go to the dealer.
   record_.append(true, points);
   observed_->on_starter_revealed(game, starter, points);
}

void ScoreLogger::on_play(const GameView& game, Play play, int points)
{
   record_.append((play.player == game.dealer()), points);
   observed_->on_play(game, play, points);
}

void ScoreLogger::on_hand_show(const GameView& game,
                               PlayerIndex player,
                               const CardsPlayed& hand,
                               int points)
{
   record_.append((player == game.dealer()), points);
   observed_->on_hand_show(game, player, hand, points);
}

void ScoreLogger::on_crib_show(const GameView& game,
                               const CardsInCrib& crib,
                               int points)
{
   // Crib points always go to the dealer.
   record_.append(true, points);

   // Round is over, so append the completed record to the log.
   log_->append(record_);
   record_.clear();

   observed_->on_crib_show(game, crib, points);
}

void ScoreLogger::on_game_over(const GameView& game, PlayerIndex winner)
{
   // We don't save partial records. There are roughly 9 rounds per game, so
   // we end up discarding about 11% of our data. Theoretically, we could work
   // around this by making points per game configurable, but it's not worth it
   // for a small performance gain.
   record_.clear();
   observed_->on_game_over(game, winner);
}
