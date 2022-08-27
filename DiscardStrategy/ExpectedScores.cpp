#include "ExpectedScores.h"
#include "FileIO.h"

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
