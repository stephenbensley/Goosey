#ifndef DiscardSimulator_h
#define DiscardSimulator_h

#include "DiscardTable.h"
#include "ExpectedScores.h"
#include "HandVsHand.h"
#include "Spinlock.h"
#include <array>
#include <unordered_map>

// Simulates every possible discard action vs. a given opponent strategy and
// collects various statistics.
class DiscardSimulator
{
public:
   DiscardSimulator(const DiscardTable& opponent,
                    const HandVsHand& hvh) noexcept;

   // Simulate the hands. May be called multiple times to split up a long
   // simulation into chunks.
   void simulate(int64_t num_hands);

   // Calculates the best response to the opponent's strategy. Return value is
   // the exploitability of the opponent's strategy in points.
   double best_response(DiscardTable& response) const noexcept;

   // Calculates the expected scores against the opponent's strategy.
   void expected_scores(ExpectedScores& results) const noexcept;

   // Load/save the simulation results from/to a file. Note: this doesn't
   // preserve the DiscardTable or HandVsHand data.
   bool load(const char* filename);
   void save(const char* filename) const noexcept;

private:
   // Tallies the results of a discard action. Based on experiments, it would
   // take >10^13 hands to overflow a signed int.
   struct ActionResult {
      int observer_play_points = 0;
      int observer_hand_points = 0;
      int opponent_play_points = 0;
      int opponent_hand_points = 0;

      ActionResult& operator+=(const ActionResult& rhs) noexcept;
      // Net points scored by the observer.
      int observer_net_points() const noexcept;
   };
   using ActionResults = std::array<ActionResult, num_discard_actions>;

   // Tracks the results for each discard action vs. a given hand. There are a
   // large number of hands, so contention on a specific hand should be light;
   // hence, we use a spinlock to serialize access.
   struct State {
      Spinlock lock;
      int count = 0;
      ActionResults results;

      // Calculates the ExpectedScores based on the current data.
      ExpectedScores::Scores expected_scores() const noexcept;
   };
   
   // Tracks the valid discard actions for a given hand and the results.
   // Results will be different depending on whether the observer is dealer or
   // pone.
   struct Entry {
      State dealer;
      State pone;
   };

   void simulate_worker(int64_t num_hands) noexcept;

   // Find the best action given the simulated results. Returns the best action
   // and the number of points scored by the action.
   static std::pair<int, int> find_best(const ActionResults& results) noexcept;

    // Entries for every possible equivalence class of hands.
   using MapType = std::unordered_map<uint64_t, Entry>;
   MapType entries_;
   // Opponent strategy used for the simulation.
   const DiscardTable& opponent_;
   // Used to lookup the card play value of the kept cards.
   const HandVsHand& hvh_;
};

#endif /* DiscardSimulator_h */
