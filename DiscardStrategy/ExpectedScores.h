#ifndef ExpectedScores_h
#define ExpectedScores_h

#include "DiscardDefs.h"
#include <array>
#include <cassert>
#include <cstdint>
#include <unordered_map>

// Table of expected scores for each possible discard action for every
// possible Cribbage hand.
class ExpectedScores
{
public:
   // Expected points scored in milli-points (i.e., thousandths of a point)
   struct Score {
      int16_t observer_play_mpoints = 0;
      int16_t observer_hand_mpoints = 0;
      int16_t opponent_play_mpoints = 0;
      int16_t opponent_hand_mpoints = 0;

      int observer_net_mpoints() const noexcept;
   };
   using Scores = std::array<Score, num_discard_actions>;

   // All the state for a given hand. Expected points depend on whether the
   // observer is dealer or pone.
   struct Entry {
      Scores dealer;
      Scores pone;
   };

   // Returns the entry for the hand with the given key.
   const Entry& find(uint64_t key) const noexcept;

   // Returns true if an entry exists for the given key.
   bool contains(uint64_t key) const noexcept;

   // Load/save the results from/to a file.
   bool load(const char* filename);
   void save(const char* filename) const noexcept;

   // Clears the lookup table.
   void clear() noexcept;

   // Add a new entry to the table. If an entry already exists for the given
   // key, it is silently overwritten.Ex
   void insert(uint64_t key, const Entry& value);

private:
   using MapType = std::unordered_map<uint64_t, Entry>;
   MapType results_;
};

inline const ExpectedScores::Entry&
ExpectedScores::find(uint64_t key) const noexcept
{
   auto i = results_.find(key);
   assert(i != results_.end());
   return i->second;
}

inline bool ExpectedScores::contains(uint64_t key) const noexcept
{
   return results_.find(key) != results_.end();
}

inline void ExpectedScores::clear() noexcept
{
   results_.clear();
}

inline void ExpectedScores::insert(uint64_t key, const Entry& value)
{
   assert(!contains(key));
   results_[key] = value;
}

#endif /* ExpectedScores_h */
