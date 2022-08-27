#include "MinimaxStrategy.h"
#include "SizedArray.h"
#include <algorithm>
#include <limits>

int minimax(const CardPlayNode& node)
{
   if (node.is_terminal()) {
      return node.result();
   }
   int value;
   if (node.is_current_player()) {
      value = std::numeric_limits<int>::min();
      for (auto play : node.valid_plays()) {
         value = std::max(value, minimax(node[play]));
      }
   } else {
      value = std::numeric_limits<int>::max();
      for (auto play : node.valid_plays()) {
         value = std::min(value, minimax(node[play]));
      }
   }
   return value;
}

void MinimaxStrategy::start_new_round(const RanksInHand& hand,
                                         bool dealer) noexcept
{
   game_.start_new_round(hand, dealer);

   ranks_seen_.clear();
   std::for_each(hand.begin(), hand.end(), [this](auto rank) {
      ranks_seen_ += rank;
   });
}

Rank MinimaxStrategy::get_rank_to_play() noexcept
{
   auto rank = solve();
   game_.make_play(rank);
   return rank;
}

void MinimaxStrategy::on_opponent_play(Rank rank) noexcept
{
   if (rank == go_rank) {
      if (game_.unseen_card_count() > 0) {
         // Opponent has cards left, but none of them could be played, so we
         // know he doesn't have any of the ranks that could have be played.
         auto max_rank = max_count_in_play - game_.count();
         assert(max_rank >= min_card_rank);
         assert(max_rank < max_card_value);
         for (auto i = min_card_rank; i <= max_rank; ++i) {
            ranks_seen_.add_all(i);
         }
      }
   } else {
      ranks_seen_ += rank;
   }
   game_.make_play(rank, true);
}

Rank MinimaxStrategy::solve() noexcept
{
   assert(!game_.is_terminal());
   assert(game_.is_current_player());

   auto plays = game_.valid_plays();
   if (plays.size() == 1) {
      return plays[0];
   }
   
   SizedArray<int, num_cards_in_hand> results(plays.size());
   CardPlayHandsIterator hands(game_.unseen_card_count(), ranks_seen_);
   while (hands.next()) {
      game_.randomize(hands.hand());
      for (auto j = 0; j < plays.size(); ++j) {
         results[j] += minimax(game_[plays[j]]) * hands.combos();
      }
   }

   auto i = std::max_element(results.begin(), results.end());
   return plays[std::distance(results.begin(), i)];
}
