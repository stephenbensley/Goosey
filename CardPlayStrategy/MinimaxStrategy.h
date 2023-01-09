//
// Copyright 2022 Stephen E. Bensley
//
// This file is licensed under the MIT License. You may obtain a copy of the
// license at https://github.com/stephenbensley/Goosey/blob/main/LICENSE.
//

#ifndef ExpectimaxStrategy_h
#define ExpectimaxStrategy_h

#include "CardPlayHands.h"
#include "CardPlayNode.h"

// Implements the expectimax algorithm to select a card to play.
class MinimaxStrategy
{
public:
   // Prepare to play a new round.
   void start_new_round(const RanksInHand& hand, bool dealer) noexcept;

   // Remove the rank from play when randomizing the opponent's hand.
   void on_rank_seen(Rank rank) noexcept;

   // Returns the rank of the card to play.
   Rank get_rank_to_play() noexcept;

   // Notify the opponent's move.
   void on_opponent_play(Rank rank) noexcept;

private:
   // Our current node in the game tree.
   CardPlayNode game_;
   // Count of all ranks that are accounted for, i.e., these ranks can't be in
   // the opponent's hand.
   RankCounts ranks_seen_;

   // Solve for the next move.
   Rank solve() noexcept;
};

inline void MinimaxStrategy::on_rank_seen(Rank rank) noexcept
{
   ranks_seen_ += rank;
}

#endif /* MinimaxSolver_h */
