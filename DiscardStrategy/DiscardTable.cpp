//
// Copyright 2022 Stephen E. Bensley
//
// This file is licensed under the MIT License. You may obtain a copy of the
// license at https://github.com/stephenbensley/Goosey/blob/main/LICENSE.
//

#include "DiscardTable.h"
#include "Canonize.h"
#include "CardSplitter.h"
#include "FileIo.h"
#include "Score.h"

void DiscardTable::format_actions(std::ostream& out,
                                         const CardsDealt& cards) const noexcept
{
   // Canonize the hand.
   CardsDealt canonized(cards);
   auto key = canonize(canonized);

   // Retrieve the actions.
   auto actions = find(key);

   CardSplitter splitter(canonized);

   out << "Hand: " << to_string(canonized.begin(), canonized.end()) << '\n';
   out << "\n                     dealer   pone\n";

   for (auto i = 0; i < num_discard_actions; ++i) {
      out << to_string(splitter.hand.begin(), splitter.hand.end()) << " - "
          << to_string(splitter.crib.begin(), splitter.crib.end());

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

bool DiscardTable::load(const char* filename)
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

void DiscardTable::save(const char* filename) const noexcept
{
   std::ofstream ostrm(filename, std::ios::binary | std::ios::trunc);
   write_pod_map(ostrm, strategy_);
}

void DiscardTable::insert(uint64_t key, int dealer_action, int pone_action)
{
    strategy_[key] = { dealer_action, pone_action };
}

DiscardTable generate_greedy_strategy()
{
   DiscardTable dst;
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
