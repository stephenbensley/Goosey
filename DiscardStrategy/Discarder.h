//
// Copyright 2022 Stephen E. Bensley
//
// This file is licensed under the MIT License. You may obtain a copy of the
// license at https://github.com/stephenbensley/Tensity/blob/main/LICENSE.
//

#ifndef Discarder_h
#define Discarder_h

#include "CardSplitter.h"
#include "DiscardTable.h"
#include "GameView.h"
#include "PlayerIndex.h"

// Abstract interface for implementing Cribbage discard logic.
class Discarder
{
public:
   virtual ~Discarder() = default;

   // Set the index for this instance. May only be called once.
   void set_index(PlayerIndex new_value);

   // Helper functions to query game state.
   PlayerIndex index() const noexcept;
   bool is_dealer(const GameView& game) const noexcept;

   virtual CardSplitter get_discards(const GameView& game,
                                     const CardsInHand& hand) = 0;

protected:
   Discarder() = default;
   Discarder(const Discarder&) = default;
   Discarder& operator=(const Discarder&) = default;

private:
   PlayerIndex index_ = invalid_player;
};

// Discards randomly.
class RandomDiscarder : public Discarder
{
public:
   virtual CardSplitter get_discards(const GameView& game,
                                     const CardsInHand& hand) override;
};

// Discards whichever cards will give the highest guaranteed point total at
// show. Does not consider starter card or opponent's discards.
class GreedyDiscarder : public Discarder
{
public:
   virtual CardSplitter get_discards(const GameView& game,
                                     const CardsInHand& hand) override;
};

// Discards based on a table look-up.
class TableDiscarder : public Discarder
{
public:
   explicit TableDiscarder(const char* datafile);

   virtual CardSplitter get_discards(const GameView& game,
                                     const CardsInHand& hand) override;

private:
   DiscardTable strategy_;
};

inline PlayerIndex Discarder::index() const noexcept
{
   return index_;
}

inline bool Discarder::is_dealer(const GameView& game) const noexcept
{
   return game.dealer() == index();
}

#endif /* Discarder_h */
