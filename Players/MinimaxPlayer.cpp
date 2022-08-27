#include "MinimaxPlayer.h"
#include "Canonize.h"
#include "CardSplitter.h"
#include "MinimaxStrategy.h"

MinimaxPlayer::MinimaxPlayer() noexcept
{
   discard_.load(filename);
}

std::unique_ptr<Player> MinimaxPlayer::clone() const
{
    return std::make_unique<MinimaxPlayer>(*this);
}

CardsDiscarded MinimaxPlayer::get_discards(const GameView& game,
                                           const CardsInHand& hand)
{
   CardsDealt canonical(hand.data());
   auto actions = discard_.find(canonize(canonical));
   CardSplitter splitter(canonical);
   splitter.seek(is_dealer(game) ? actions.dealer : actions.pone);

   RanksInHand ranks;
   for (auto card : splitter.hand) {
      ranks.push_back(card.rank());
   }
   card_play_.start_new_round(ranks, is_dealer(game));
   for (auto card : splitter.crib) {
      card_play_.on_rank_seen(card.rank());
   }

   return splitter.crib;
}

Card MinimaxPlayer::get_card_to_play(const GameView& game,
                                     const CardsInHand& hand)
{
   auto rank = card_play_.get_rank_to_play();
   if (rank != 0) {
      auto i = std::find_if(hand.begin(), hand.end(), [rank](auto c) {
         return c.rank() == rank;
      });
      assert(i != hand.end());
      return *i;
   }
   return go_card;
}

void MinimaxPlayer::on_starter_revealed(const GameView& game,
                                        Card starter,
                                        int points)
{
   card_play_.on_rank_seen(starter.rank());
}

void MinimaxPlayer::on_play(const GameView& game, Play play, int points)
{
   if (play.player != index()) {
      card_play_.on_opponent_play(play.card.rank());
   }
}
