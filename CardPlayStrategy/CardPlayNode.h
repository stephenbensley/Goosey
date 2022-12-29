//
// Copyright 2022 Stephen E. Bensley
//
// This file is licensed under the MIT License. You may obtain a copy of the
// license at https://github.com/stephenbensley/Tensity/blob/main/LICENSE.
//

#ifndef CardPlayGame_h
#define CardPlayGame_h

#include "CardPlayModel.h"

// Represents a node in the card play game tree. Provides an abstraction
// suitable for tree-search alorithms.
class CardPlayNode
{
public:
   // Starts a new round with the specified state for the observer. You
   // must call randomize to initialize the opponent's state.
   void start_new_round(const RanksInHand& hand, bool dealer) noexcept;
   // Returns number of unseen cards in the opponent's hand.
   int unseen_card_count() const noexcept;
   // Randomize the unseen cards in the opponent's hand.
   void randomize(const RanksInHand& unseen) noexcept;
   // Returns true if the observer is the current player.
   bool is_current_player() const noexcept;
   // Running point count for the cards on the table.
   int count() const noexcept;
   // Returns the set of valid moves from the current node.
   RanksInHand valid_plays() const noexcept;
   // If force is true, makes the play even if the card isn't found in the
   // current hand. This is used when updating the game state based on an
   // opponent's actual play; you must then call randomize to reinitialize the
   // opponent's hand.
   void make_play(Rank play, bool force = false) noexcept;
   // Returns true if we're at a terminal node, i.e., the round is over.
   bool is_terminal() const noexcept;
   // The cumulative net points scored when this node is reached.
   int result() const noexcept;
   // Perform the specified play and return the new game state.
   CardPlayNode operator[](Rank play) const noexcept;

private:
   static constexpr PlayerIndex observer_index = 0;

   RanksInHand& current_hand() noexcept;
   const RanksInHand& current_hand() const noexcept;

   RanksInHand observer_;
   RanksInHand opponent_;
   CardPlayModel model_;
   int result_ = 0;
};

inline int CardPlayNode::unseen_card_count() const noexcept
{
   return static_cast<int>(opponent_.size());
}

inline bool CardPlayNode::is_current_player() const noexcept
{
   return model_.current_player() == observer_index;
}

inline int CardPlayNode::count() const noexcept
{
   return model_.count();
}

inline bool CardPlayNode::is_terminal() const noexcept
{
   return model_.round_over();
}

inline int CardPlayNode::result() const noexcept
{
   return result_;
}

inline CardPlayNode CardPlayNode::operator[](Rank play) const noexcept
{
   CardPlayNode child(*this);
   child.make_play(play);
   return child;
}

inline RanksInHand& CardPlayNode::current_hand() noexcept
{
   return is_current_player() ? observer_ : opponent_;
}

inline const RanksInHand& CardPlayNode::current_hand() const noexcept
{
   return is_current_player() ? observer_ : opponent_;
}

#endif /* CardPlayGame_h */
