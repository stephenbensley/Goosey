//
// Copyright 2022 Stephen E. Bensley
//
// This file is licensed under the MIT License. You may obtain a copy of the
// license at https://github.com/stephenbensley/Tensity/blob/main/LICENSE.
//

#include "CardPlayScore.h"
#include "FileIo.h"
#include "Score.h"

const CardPlayScores::ScoreMap& CardPlayScores::get()
{
   static CardPlayScores scores;
   return scores.scores_;
}

CardPlayScores::ScoreNode::ScoreNode() noexcept
{
   remaining.fill(num_card_suits);
}

bool CardPlayScores::ScoreNode::try_push(int rank) noexcept
{
   // Suits don't count during card play, so pick any.
   Card card(rank, 1);
   // Can only play the card if all the following are true:
   //   1) It won't push the count over 31.
   //   2) There's at least one card of that rank remaining in the deck.
   //   3) All the cards for the round haven't already been played.
   if (((card.value() + count) > max_count_in_play) ||
       (remaining[card.rank() - min_card_rank] == 0) ||
       (cards.size() == cards.capacity())) {
      return false;
   }

   // Now that we know the push will succeed, update all our state.
   key.push_back(card.rank());
   count += card.value();
   --remaining[card.rank() - min_card_rank];
   cards.push_back(card);
   return true;
}

void CardPlayScores::ScoreNode::pop() noexcept
{
   // Undo everything we did in the previous function.
   assert(!cards.empty());
   auto card = cards.back();
   cards.pop_back();
   ++remaining[card.rank() - min_card_rank];
   count -= card.value();
   key.pop_back();
}

CardPlayScores::CardPlayScores()
{
   if (!load()) {
      // If we can't load the cached table, build a new one and save it.
      ScoreNode root;
      evaluate(root);
      save();
   }
}


void CardPlayScores::evaluate(ScoreNode& node)
{
   // Process this node.
   scores_[node.key()] = score_cards_in_play(node.cards.begin(),
                                             node.cards.end(),
                                             node.count);;
   // Then traverse all the children.
   for (auto i = min_card_rank; i <= max_card_rank; ++i) {
      if (node.try_push(i)) {
         evaluate(node);
         node.pop();
      }
   }
}

bool CardPlayScores::load() noexcept
{
   std::ifstream istrm(filename, std::ios::binary);
   if (!istrm.is_open()) {
      return false;
   }
   ScoreMap tmp;
   if (!read_pod_map(istrm, tmp)) {
      return false;
   }
   if (!read_complete(istrm)) {
      return false;
   }
   scores_.swap(tmp);
   return true;

}

void CardPlayScores::save() const noexcept
{
   std::ofstream ostrm(filename, std::ios::binary | std::ios::trunc);
   write_pod_map(ostrm, scores_);
}
