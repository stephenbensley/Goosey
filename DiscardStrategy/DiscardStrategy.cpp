#include "DiscardStrategy.h"
#include "Canonize.h"
#include "CardSplitter.h"
#include "FileIo.h"
#include "Score.h"

#include "DiscardActions.h"

CardsDiscarded
PureDiscardStrategy::get_action(bool dealer,
                                const CardsDealt& cards) const noexcept
{
   // Make a copy of the hand, so we can canonize it.
   CardsDealt canonical(cards);
   // Canonize the hand and lookup the actions based on the canonical key.
   auto actions = get_actions(canonize(canonical));
   // Load a CardSplitter to evaluate the chosen action.
   CardSplitter splitter(canonical);
   splitter.seek(dealer ? actions.dealer : actions.pone);
   // Return the discards.
   return splitter.crib;
}

void PureDiscardStrategy::format_actions(std::ostream& out,
                                         const CardsDealt& cards) const noexcept
{
   // Canonize the hand.
   CardsDealt canonized(cards);
   auto key = canonize(canonized);

   // Retrieve the actions.
   auto actions = get_actions(key);
   DiscardActions available(canonized);

   CardSplitter splitter(canonized);

   out << "Hand: " << to_string(canonized.begin(), canonized.end()) << '\n';
   out << "\n                        dealer   pone\n";

   for (auto i = 0; i < num_discard_actions; ++i) {
      out << to_string(splitter.hand.begin(), splitter.hand.end()) << " - "
          << to_string(splitter.crib.begin(), splitter.crib.end());
      if (available.test(i)) {
         out << " + ";
      } else {
         out << " - ";
      }
      if (i == actions.dealer) {
         out << "     * ";
      } else {
         out << "       ";
      }
      if (i == actions.pone) {
         out << "      * \n";
      } else {
         out << '\n';
      }
      splitter.next();
   }
}

bool PureDiscardStrategy::load(const char* filename) noexcept
{
   std::ifstream istrm(filename, std::ios::binary);
   if (!istrm.is_open()) {
      return false;
   }
   MapType tmp;
   if (!read_pod_map(istrm, tmp)) {
      return false;
   }
   if (!read_complete(istrm)) {
      return false;
   }
   strategy_.swap(tmp);
   return true;
}

void PureDiscardStrategy::save(const char* filename) const noexcept
{
   std::ofstream ostrm(filename, std::ios::binary | std::ios::trunc);
   write_pod_map(ostrm, strategy_);
}

void PureDiscardStrategy::insert(uint64_t key,
                                 int dealer_action,
                                 int pone_action)
{
    strategy_[key] = { dealer_action, pone_action };
}

PureDiscardStrategy generate_greedy_strategy()
{
   PureDiscardStrategy dst;
   generate_cards_dealt([&dst](auto& cards) {
      auto key = canonize(cards);
      if (dst.contains(key)) { return; }

      CardSplitter splitter(cards);

      auto max_dealer_points = -1;
      auto best_dealer_action = 0;

      auto max_pone_points = -1;
      auto best_pone_action = 0;

      do  {
         auto hand_points = score_hand(splitter.hand.begin(),
                                       splitter.hand.end(),
                                       nullcard,
                                       false);
         auto crib_points = score_hand(splitter.crib.begin(),
                                       splitter.crib.end(),
                                       nullcard,
                                       true);

         auto dealer_points = hand_points + crib_points;
         if (dealer_points > max_dealer_points) {
            max_dealer_points = dealer_points;
            best_dealer_action = splitter.pos();
         }

         auto pone_points = hand_points - crib_points;
         if (pone_points > max_pone_points) {
            max_pone_points = pone_points;
            best_pone_action = splitter.pos();
         }
      } while (splitter.next());

      dst.insert(key, best_dealer_action, best_pone_action);
   });

   return dst;
}
