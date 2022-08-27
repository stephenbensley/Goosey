#ifndef DiscardBestResponse_h
#define DiscardBestResponse_h

#include "DiscardActions.h"
#include "DiscardStrategy.h"
#include "HandVsHand.h"
#include "Spinlock.h"
#include <array>
#include <unordered_map>

// Computes the best response to a discard strategy.
class DiscardBestResponse
{
public:
   explicit DiscardBestResponse(const PureDiscardStrategy& strategy,
                                const HandVsHand& hvh) noexcept;

   // Compute the best response strategy.
   void compute(uint64_t num_iterations);

   // Returns the exploitability of the strategy in points per round.
   double exploitability() const noexcept;
   // Returns the best response strategy.
   const PureDiscardStrategy& best_response() const noexcept;

private:
   void compute_worker(uint64_t num_iterations) noexcept;

   // Tracks the number of points each discard action would have scored for a
   // given hand. There are a large number of hands, so contention on a specific
   // hand should be light; hence, we use a spinlock to serialize access.
   struct State {
      Spinlock lock;
      std::array<int, num_discard_actions> points{};
   };
   // Tracks the valid discard actions for a given hand and the points scored.
   struct Entry {
      DiscardActions actions;
      State dealer;
      State pone;
   };

   // Entries for every possible equivalence class of hands.
   std::unordered_map<uint64_t, Entry> results_;
   // Strategy under analysis.
   const PureDiscardStrategy& strategy_;
   // Used to lookup the card play value of the kept cards.
   const HandVsHand& hvh_;
   // Computed best response.
   PureDiscardStrategy response_;
   // Exploitability in points per round of the analyzed strategy.
   double exploitability_ = 0.0;
};

inline double DiscardBestResponse::exploitability() const noexcept
{
   return exploitability_;
}

inline const PureDiscardStrategy& DiscardBestResponse::best_response() const noexcept
{
   return response_;
}

#endif /* DiscardBestResponse_h */
