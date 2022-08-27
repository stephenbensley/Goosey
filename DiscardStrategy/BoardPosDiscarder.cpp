#include "BoardPosDiscarder.h"
#include "Canonize.h"

BoardPositionDiscarder::BoardPositionDiscarder(const char* boardvalue,
                                               const char* expectedscores)
{
   board_.load(boardvalue);
   scores_.load(expectedscores);
}

CardSplitter BoardPositionDiscarder::get_discards(const GameView& game,
                                                  const CardsInHand& hand)
{
   // Look up the expected scores for the hand and the observer's role.
   CardsDealt canonical(hand.data());
   auto& entry = scores_.find(canonize(canonical));
   auto& scores = is_dealer(game) ? entry.dealer : entry.pone;

   // Find the action that yields the max net points.
   auto max_points = std::numeric_limits<int>::min();
   auto best_action = -1;
   for (auto i = 0; i < scores.size(); ++i) {
      auto net_points = scores[i].observer_net_mpoints();
      if (net_points > max_points) {
         max_points = net_points;
         best_action = i;
      }
   }

   assert(best_action >= 0);
   CardSplitter splitter(canonical);
   splitter.seek(best_action);
   return splitter;
}
