#include "ScoreLog.h"
#include "FileIO.h"
#include <cassert>

void ScoreRecord::append(bool dealer, int points) noexcept
{
   if (points > 0) {
      // Dealer uses the even numbered entries. Are we already positioned on
      // the correct entry?
      if ((dealer ? 0 : 1) == (pos_ % num_players)) {
         // Yes, so just add the points.
         points_[pos_] += points;
      } else {
         // No, so advance to the next entry.
         ++pos_;
         assert(pos_ < points_.size());
         points_[pos_] = points;
      }
   }
}

ScoreRecord::Result ScoreRecord::apply(const Score& start) const noexcept
{
   Result result = { invalid_player, start };
   for (auto i = 0; i < size(); ++i) {
      auto player = (i % num_players);
      result.score[player] += points_[i];
      if (result.score[player] >= num_points_to_win) {
         result.winner = player;
         result.score[player] = num_points_to_win;
         break;
      }
   }
   return result;
}

void ScoreLog::append(const ScoreRecord& record)
{
   std::lock_guard<std::mutex> guard(lock_);
   records_.push_back(record);
}

bool ScoreLog::load(const char* filename)
{
   std::ifstream istrm(filename, std::ios::binary);
   if (!istrm.is_open()) {
      return false;
   }
   LogType tmp;
   if (!read_pod_vector(istrm, tmp)) {
      return false;
   }
   if (!read_complete(istrm)) {
      return false;
   }
   records_.swap(tmp);
   return true;

}

void ScoreLog::save(const char* filename) const noexcept
{
   std::ofstream ostrm(filename, std::ios::binary | std::ios::trunc);
   write_pod_vector(ostrm, records_);
}
