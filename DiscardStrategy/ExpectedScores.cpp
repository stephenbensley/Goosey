#include "ExpectedScores.h"
#include "FileIO.h"

int ExpectedScores::Score::observer_mpoints() const noexcept
{
   return static_cast<int>(observer_play_mpoints) +
          static_cast<int>(observer_hand_mpoints);
}

int ExpectedScores::Score::opponent_mpoints() const noexcept
{
   return static_cast<int>(opponent_play_mpoints) +
          static_cast<int>(opponent_hand_mpoints);
}

int ExpectedScores::Score::observer_net_mpoints() const noexcept
{
   return observer_mpoints() - opponent_mpoints();
}

bool ExpectedScores::load(const char* filename)
{
   std::ifstream istrm(filename, std::ios::binary);
   if (!istrm.is_open()) {
      return false;
   }
   MapType tmp;
   if (!read_pod_map(istrm, tmp)) {
      return false;
   }
   if (!read_complete(istrm)) {
      return false;
   }
   results_.swap(tmp);
   return true;
}

void ExpectedScores::save(const char* filename) const noexcept
{
   std::ofstream ostrm(filename, std::ios::binary | std::ios::trunc);
   write_pod_map(ostrm, results_);

}
