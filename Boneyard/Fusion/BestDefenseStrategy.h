#ifndef BestDefenseStrategy_h
#define BestDefenseStrategy_h

#include "CardPlayHands.h"
#include "CardPlayNode.h"
#include "InfoSet.h"

// Implements the expectimax algorithm to select a card to play.
class BestDefenseStrategy
{
public:
   // Prepare to play a new round.
   void start_new_round(const RanksInHand& hand,
                        const RanksDiscarded& discards,
                        bool dealer) noexcept;

   void on_starter_revealed(Rank starter) noexcept;

   // Returns the rank of the card to play.
   Rank get_rank_to_play() noexcept;

   // Notify the opponent's move.
   void on_opponent_play(Rank rank) noexcept;

   InfoSets info_sets_;

private:
   // Our current node in the game tree.
   CardPlayNode game_;
   // Count of all ranks that are accounted for, i.e., these ranks can't be in
   // the opponent's hand.
   RankCounts ranks_seen_;
   bool fuseStrategies = true;
   bool searchLocally = true;

   static constexpr int max_iterations = 5;

   // Solve for the next move.
   void traverse() noexcept;
   void solve() noexcept;
   int search(const CardPlayNode& node, int weight);
};

#endif /* BestDefenseStrategy_h */
