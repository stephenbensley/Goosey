//
// Copyright 2022 Stephen E. Bensley
//
// This file is licensed under the MIT License. You may obtain a copy of the
// license at https://github.com/stephenbensley/Tensity/blob/main/LICENSE.
//

#ifndef PlayerIndex_h
#define PlayerIndex_h

#include "CribDefs.h"
#include <array>

// When examining the game state, each player needs to know which player they
// are. This is equivalent to knowing the color of your pieces in a board game.
using PlayerIndex = int;

constexpr PlayerIndex invalid_player = -1;

inline bool is_valid_player(PlayerIndex idx) noexcept
{
   return (idx >= 0) && (idx < num_players);
}

// Returns the next player in rotation. Useful for passing the deal, etc.
inline PlayerIndex next_player(PlayerIndex idx) noexcept
{
   return (idx + 1) % num_players;
}

// Similar to next_player, but assumes there are exactly two players.
inline PlayerIndex other_player(PlayerIndex idx) noexcept
{
   static_assert(num_players == 2);
   return idx ? 0 : 1;
}

inline
std::array<PlayerIndex, num_players> deal_order(PlayerIndex dealer) noexcept
{
   static_assert(num_players == 2);
   return { other_player(dealer), dealer };
}

#endif /* PlayerIndex_h */
