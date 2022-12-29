//
// Copyright 2022 Stephen E. Bensley
//
// This file is licensed under the MIT License. You may obtain a copy of the
// license at https://github.com/stephenbensley/Tensity/blob/main/LICENSE.
//

#include "BoardValue.h"
#include "FileIO.h"
#include <cmath>
#include <future>

std::optional<double> BoardValue::p_win(double x, double y) const noexcept
{
   // Use bilinear interpolation to estimate the win probability for
   // fractional values.

   // Find the four corners around the point at which we want to interpolate.
   int x1 = std::floor(x);
   auto x2 = x1 + 1;
   int y1 = std::floor(y);
   auto y2 = y1 + 1;

   // If we're out of range, there's nothing we can do.
   if ((x2 >= num_points_to_win) || (y2 >= num_points_to_win))
   {
      return std::optional<double>();
   }

   // Look up the win probabilities at the corners.
   auto f11 = value_[x1][y1];
   auto f12 = value_[x1][y2];
   auto f21 = value_[x2][y1];
   auto f22 = value_[x2][y2];

   // Lifted from the Wikipedia article on bilinear interpolation.
   return ((x2 - x) * f11 + (x - x1) * f21) * (y2 - y) +
          ((x2 - x) * f12 + (x - x1) * f22) * (y - y1);
}

void BoardValue::build(const ScoreLog& log)
{
   const auto concurrency = std::thread::hardware_concurrency();
   // Max points that can occur in an active game. Can't actually have
   // num_points_to_win since the game would be over.
   constexpr int max_points = num_points_to_win - 1;

   // We start at 240 total points (120 vs. 120) and iterate to zero total
   // points (0 vs. 0). Since scores are always non-negative and since each
   // round at least one point is scored, each iteration can only end in a
   // score that has already been evaluated.
   for (auto total = max_points * 2; total >= 0; --total) {
      // Highest and lowest possible scores for this total.
      auto hi = std::min(max_points, total);
      auto lo = total - hi;

      // Iterate through every possible combination of scores that yields this
      // total and divvy them up among the workers.
      std::vector<Scores> per_worker(concurrency);
      for (auto i = lo; i <= hi; ++i) {
         per_worker[i % concurrency].push_back({ i, (total - i) });
      }

      // Launch the workers and give each his share of the cells.
      std::vector<std::future<void>> futures;
      for (auto i = 0; i < concurrency; ++i) {
         futures.push_back(std::async(std::launch::async,
                                      &BoardValue::compute_cells,
                                      this,
                                      std::ref(per_worker[i]),
                                      std::ref(log)));
      }

      // Wait for all workers to complete before beginning the next iteration.
      std::for_each(futures.begin(), futures.end(), [](auto& f){ f.get(); });
   }
}

bool BoardValue::load(const char* filename)
{
   std::ifstream istrm(filename, std::ios::binary);
   if (!istrm.is_open()) {
      return false;
   }
   auto bytes = sizeof(value_);
   std::vector<char> tmp(bytes);
   if (!istrm.read(tmp.data(), bytes)) {
      return false;
   }
   if (!read_complete(istrm)) {
      return false;
   }
   memcpy(value_, tmp.data(), bytes);
   return true;
}

void BoardValue::save(const char* filename) const noexcept
{
   std::ofstream ostrm(filename, std::ios::binary | std::ios::trunc);
   ostrm.write(reinterpret_cast<const char*>(value_), sizeof(value_));
}

void BoardValue::compute_cell(const Score& cell, const ScoreLog& log) noexcept
{
   double value = 0.0;
   for (auto i = log.records().begin(); i != log.records().end(); ++i) {
      auto result = i->apply(cell);
      if (result.winner == 0) {
         value += 1.0;
      } else if (result.winner == invalid_player) {
         // In the next round, the dealer and pone are reversed, so we have
         // to reverse the lookup of the win probability.
         value += (1.0 - value_[result.score[1]][result.score[0]]);
      }
      // If the pone wins, we add 0.0 to the value.
   }
   // Normalize the probability based on the number of records processed.
   value /= log.records().size();
   value_[cell[0]][cell[1]] = value;
}

void BoardValue::compute_cells(const Scores& cells, const ScoreLog& log) noexcept
{
   std::for_each(cells.begin(), cells.end(), [this, &log](auto& cell) {
      compute_cell(cell, log);
   });
}

