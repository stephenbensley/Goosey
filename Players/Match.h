#ifndef Match_h
#define Match_h

#include "Player.h"
#include <array>
#include <cassert>

// Stores the results of a cribbage match.
struct MatchResults
{
   // Games won by each player.
   std::array<int, num_players> wins;
   // microseconds per game
   int usec_per_game;
   // core-microseconds per game
   int core_usec_per_game;

   // Aggregates MatchResults from multiple sources.
   MatchResults& operator+=(const MatchResults rhs) noexcept;
};

// Conducts a cribbage match between two player types.
class Match
{
public:
   explicit Match(const Players& players) noexcept;

   MatchResults play(int num_games) const;

private:
   // Worker for each parallel game.
   MatchResults play_worker(int num_games) const;

   Players players_;
};

#endif /* Match_h */
