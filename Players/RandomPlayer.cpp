#include "RandomPlayer.h"
#include <algorithm>

std::unique_ptr<Player> RandomPlayer::clone() const
{
    return std::make_unique<RandomPlayer>(*this);
}

CardsDiscarded RandomPlayer::get_discards(const GameView& game,
                                          const CardsInHand& hand)
{
   CardsDiscarded discards;
   for (auto i = 0; i < num_cards_discarded_per_player; ++i) {
      discards[i] = hand[i];
   }
   return discards;
}

Card RandomPlayer::get_card_to_play(const GameView& game,
                                    const CardsInHand& hand)
{
   auto i = std::find_if(hand.begin(), hand.end(), [&game](auto c) {
      return is_legal_play(game, c);
   });

   return (i != hand.end()) ? *i : go_card;
}
