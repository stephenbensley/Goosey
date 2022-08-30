#include "BoardPosDiscarder.h"
#include "Canonize.h"
#include <numeric>

BoardPositionDiscarder::BoardPositionDiscarder(const char* boardvalue,
                                               const char* expectedscores)
{
   board_.load(boardvalue);
   scores_.load(expectedscores);
}

double points_for_his_heels(const CardsInHand& hand) noexcept
{
   auto num_jacks = std::reduce(hand.begin(),
                                hand.end(),
                                0,
                                [](auto lhs, auto rhs) {
      return lhs + (rhs.is_jack() ? 1 : 0);
   });

   return num_points_for_his_heels *
          static_cast<double>(num_card_suits - num_jacks) /
             (num_cards_in_deck - num_cards_dealt_per_player);
}

CardSplitter BoardPositionDiscarder::get_discards(const GameView& game,
                                                  const CardsInHand& hand)
{
   auto his_heels = points_for_his_heels(hand);

   // Look up the expected scores for the hand and the observer's role.
   CardsDealt canonical(hand.data());
   auto& entry = scores_.find(canonize(canonical));
   auto& scores = is_dealer(game) ? entry.dealer : entry.pone;

   // Cache the relevant game state.
   auto dealer = is_dealer(game);
   auto observer_score = game.score(index());
   auto opponent_score = game.score(other_player(index()));

   // Find the action that yields the max value.
   auto max_value = std::numeric_limits<double>::min();
   auto best_action = -1;
   for (auto i = 0; i < scores.size(); ++i) {
      auto value = compute_value(dealer,
                                 observer_score,
                                 opponent_score,
                                 his_heels,
                                 scores[i]);
      if (!value) {
         best_action = -1;
         break;
      }

      if (*value > max_value) {
         max_value = *value;
         best_action = i;
      }
   }

   if (best_action == -1) {
      // Find the action that yields the max net points.
      auto max_points = std::numeric_limits<int>::min();
      for (auto i = 0; i < scores.size(); ++i) {
         auto net_points = scores[i].observer_net_mpoints();
         if (net_points > max_points) {
            max_points = net_points;
            best_action = i;
         }
      }
   }

   assert(best_action >= 0);
   CardSplitter splitter(canonical);
   splitter.seek(best_action);
   return splitter;
}

std::optional<double>
BoardPositionDiscarder::compute_value(bool dealer,
                                      int observer_score,
                                      int opponent_score,
                                      double his_heels,
                                      const ExpectedScores::Score& score) const noexcept
{
   auto observer_delta = score.observer_mpoints() / 1000.0;
   auto opponent_delta = score.opponent_mpoints() / 1000.0;

   auto new_observer_score = observer_score + observer_delta;
   auto new_opponent_score = opponent_score + opponent_delta;

   if (dealer) {
      auto p_win = board_.p_win(new_opponent_score,
                                new_observer_score + his_heels);
      if (p_win) {
         p_win = 1.0 - *p_win;
      }
      return p_win;
   } else {
      return board_.p_win(new_observer_score, new_opponent_score + his_heels);
   }
}
