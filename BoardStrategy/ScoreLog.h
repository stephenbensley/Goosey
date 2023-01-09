//
// Copyright 2022 Stephen E. Bensley
//
// This file is licensed under the MIT License. You may obtain a copy of the
// license at https://github.com/stephenbensley/Goosey/blob/main/LICENSE.
//

#ifndef ScoreLog_h
#define ScoreLog_h

#include "PlayerIndex.h"
#include <array>
#include <mutex>
#include <vector>

// Keeps a record of the points scored in a single hand. Consecutive points
// scored by the same player are coalesced into a single entry.
class ScoreRecord
{
public:
   void append(bool dealer, int points) noexcept;
   void clear() noexcept;
   bool empty() const noexcept;
   int size() const noexcept;

   using Score = std::array<int, num_players>;

   struct Result {
      PlayerIndex winner;
      Score score;
   };

   // Applies the ScoreRecord to the given starting position and returns the
   // outcome.
   Result apply(const Score& start) const noexcept;

private:
   // Next entry to be updated.
   short pos_ = 0;
   // Sequence of points scored, starting with the dealer at index zero. If the
   // pone is the first to score, points_[0] will be zero, and the pone's first
   // score is stored in points_[1]. Need one extra entry for each player to
   // store the result of the show.
   std::array<char, num_players * (num_cards_in_hand + 1)> points_{};
};

inline void ScoreRecord::clear() noexcept
{
   pos_ = 0;
   points_.fill(0);
}

inline bool ScoreRecord::empty() const noexcept
{
   return (pos_ == 0) && (points_[0] == 0);
}

inline int ScoreRecord::size() const noexcept
{
   return empty() ? 0 : (pos_ + 1);
}

// Keeps a log of the scores across multiple hands.
class ScoreLog
{
public:
   using LogType = std::vector<ScoreRecord>;

   void append(const ScoreRecord& record);

   const LogType& records() const noexcept;
   
   // Load/save the log from/to a file.
   bool load(const char* filename);
   void save(const char* filename) const noexcept;

   int size() const noexcept;
   void resize(int new_size);

private:

   std::mutex lock_;
   LogType records_;
};

inline const ScoreLog::LogType& ScoreLog::records() const noexcept
{
   return records_;
}

inline int ScoreLog::size() const noexcept
{
   return static_cast<int>(records_.size());
}

inline void ScoreLog::resize(int new_size)
{
   records_.resize(new_size);
}

#endif /* ScoreLog_h */
