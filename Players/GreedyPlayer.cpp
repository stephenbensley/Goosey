//
// Copyright 2022 Stephen E. Bensley
//
// This file is licensed under the MIT License. You may obtain a copy of the
// license at https://github.com/stephenbensley/Tensity/blob/main/LICENSE.
//

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
   return discarder_.get_discards(game, hand).crib;
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

void GreedyPlayer::on_set_index(PlayerIndex new_value)
{
   discarder_.set_index(new_value);
}
