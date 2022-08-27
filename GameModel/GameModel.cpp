#include "GameModel.h"
#include "HandScore.h"
#include <algorithm>


GameModel::GameModel(PlayerIndex first_deal) noexcept
: GameView(first_deal), card_play_(first_deal)
{ }

bool GameModel::reveal_starter(Card card) noexcept
{
   set_starter(card);
   // If the starter is a jack, dealer scores for 'his heels'.
   return starter().is_jack() &&
          peg_points(dealer(), num_points_for_his_heels);
}

bool GameModel::play_card(Card card) noexcept
{
   auto player = current_player();
   GameView::play_card({ player, card });
   auto points = card_play_.play_rank(card.rank());
   if (card_play_.count() == 0) {
      start_new_series();
   }
   return peg_points(player, points);
}

bool GameModel::show_hands(const CardsShown& crib) noexcept
{
   set_crib(crib);

   // Score starting with the player to the left of the dealer. Order is
   // important since whoever reaches the winning score first, wins.
   for (auto player : deal_order(dealer())) {
      if (peg_points(player, score_hand(player))) {
         return true;
      }
   }
   return peg_points(dealer(), score_crib());
}

void GameModel::start_new_round() noexcept
{
   GameView::start_new_round();
   card_play_.start_new_round(dealer());
}

int GameModel::score_hand(PlayerIndex player) const noexcept
{
   return HandScore(hand(player).begin(),
                    hand(player).end(),
                    false).score(starter());
}

int GameModel::score_crib() const noexcept
{
   return HandScore(crib().begin(), crib().end(), true).score(starter());
}
