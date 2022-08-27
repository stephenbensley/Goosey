#include "GreedyPlayer.h"
#include "CardPlayScore.h"
#include "CardSplitter.h"
#include "Score.h"

std::unique_ptr<Player> GreedyPlayer::clone() const
{
   return std::make_unique<GreedyPlayer>(*this);
}

CardsDiscarded GreedyPlayer::get_discards(const GameView& game,
                                          const CardsInHand& hand)
{
   CardSplitter splitter(hand.data());
   // Crib counts for the dealer and against the pone.
   auto crib_mult = is_dealer(game) ? +1 : -1;

   auto max_points = -1;
   CardsDiscarded best_cards;

   do  {
      auto hand_points = score_hand(splitter.hand.begin(),
                                    splitter.hand.end(),
                                    nullcard,
                                    false);
      auto crib_points = score_hand(splitter.crib.begin(),
                                    splitter.crib.end(),
                                    nullcard,
                                    true);
      auto points = hand_points + crib_mult * crib_points;
      if (points > max_points) {
         max_points = points;
         best_cards = splitter.crib;
       }
   } while (splitter.next());

   assert(max_points >= 0);
   return best_cards;
}

Card GreedyPlayer::get_card_to_play(const GameView& game,
                                    const CardsInHand& hand)
{
   CardPlayScore score;
   for (auto i = game.series_begin(); i != game.series_end(); ++i) {
      score.update(i->card.rank());
   }
   Card best_card = go_card;
   auto max_points = -1;
   for (auto c = hand.begin(); c != hand.end(); ++c) {
      if (is_legal_play(game, *c)) {
         auto points = CardPlayScore(score).update_and_score(c->rank());
         if (points > max_points) {
            max_points = points;
            best_card = *c;
         }
      }
   }
   return best_card;
}
