#ifndef BoardValue_h
#define BoardValue_h

#include "ScoreLog.h"
#include <optional>
#include <vector>

// Builds a table of the probabilities of the dealer winning the game from a
// given board position. The probabilities are stored in a 2D array. The first
// dimension is the dealer's score at the start of the round; the second
// dimension is the pone's score.
class BoardValue
{
public:
   const double* operator[](int i) const noexcept;

   // Computes the win probability for fractional scores. Returns an optional
   // since scores may be out of range.
   std::optional<double> p_win(double dealer_score,
                               double pone_score) const noexcept;


   // Builds a new table using the simulation data from the ScoreLog.
   void build(const ScoreLog& log);

   // Load/save the table from/to a file.
   bool load(const char* filename);
   void save(const char* filename) const noexcept;

private:
   using Score = ScoreRecord::Score;
   using Scores = std::vector<Score>;

   // Compute a single cell in the table.
   void compute_cell(const Score& cell, const ScoreLog& log) noexcept;
   // Compute a collection of cells.
   void compute_cells(const Scores& cells, const ScoreLog& log) noexcept;

   double value_[num_points_to_win][num_points_to_win] = {};
};

inline const double* BoardValue::operator[](int i) const noexcept
{
   return value_[i];
}

#endif /* BoardValue_h */
