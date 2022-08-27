#include "BestDefenseStrategy.h"
#include "SizedArray.h"
#include <algorithm>
#include <limits>

#include <iostream>

int BestDefenseStrategy::search(const CardPlayNode& node, int weight)
{
   if (node.is_terminal()) {
      return node.result() * weight;
   }

   auto plays = node.valid_plays();
   if (plays.size() == 1) {
      return search(node[plays.front()], weight);
   }

   if (!node.is_current_player()) {
      auto value = std::numeric_limits<int>::max();
      for (auto play : plays) {
         value = std::min(value, search(node[play], weight));
      }
      return value;
   }

   PlayValues children(plays.size());
   for (auto i = 0; i < plays.size(); ++i) {
      children[i] = search(node[plays[i]], weight);
   }

   return info_sets_.get_value(node.info_set(), children);
}

void BestDefenseStrategy::start_new_round(const RanksInHand& hand,
                                          const RanksDiscarded& discards,
                                          bool dealer) noexcept
{
   game_.start_new_round(hand, dealer);

   ranks_seen_.clear();
   std::for_each(hand.begin(), hand.end(), [this](auto rank) {
      ranks_seen_ += rank;
   });
   std::for_each(discards.begin(), discards.end(), [this](auto rank) {
      ranks_seen_ += rank;
   });
}

void BestDefenseStrategy::on_starter_revealed(Rank starter) noexcept
{
   ranks_seen_ += starter;
   if (!searchLocally) {
      solve();
   }
}

Rank BestDefenseStrategy::get_rank_to_play() noexcept
{
   auto plays = game_.valid_plays();
   Rank play;
   if (plays.size() == 1) {
      play = plays[0];
   } else {
      if (searchLocally) {
         solve();
      }
      play = info_sets_.get_rank_to_play(game_.info_set(), plays);
   }

   game_.make_play(play);
   return play;
}

void BestDefenseStrategy::on_opponent_play(Rank rank) noexcept
{
   if (rank == rank_go) {
      if (game_.unseen_card_count() > 0) {
         auto max_valid_play = game_.max_valid_play();
         assert(max_valid_play < 10);
         for (auto i = min_card_rank; i <= max_valid_play; ++i) {
            ranks_seen_.add_all(i);
         }
      }
   } else {
      ranks_seen_ += rank;
   }
   game_.make_play(rank, true);
}

void BestDefenseStrategy::traverse() noexcept
{
   CardPlayHandsIterator hands(game_.unseen_card_count(), ranks_seen_);
   while (hands.next()) {
      game_.randomize(hands.hand());
      search(game_, hands.weight());
   }
}

void BestDefenseStrategy::solve() noexcept
{
   info_sets_.clear();
   traverse();
   if (!fuseStrategies) {
      for (auto i = 0; info_sets_.update_strategy() > 0 && i < max_iterations; ++i) {
         traverse();
      }
   }
}
