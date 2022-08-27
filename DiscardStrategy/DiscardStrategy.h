#ifndef DiscardStrategy_h
#define DiscardStrategy_h

#include "Card.h"
#include <cstdint>
#include <iostream>
#include <unordered_map>

// Represents a pure strategy for selecting which cards to discard from a
// cribbage hand.
class PureDiscardStrategy
{
public:
   // Actions for each hand. Since the crib is scored for the dealer, the
   // dealer and pone will generally take different actions in the same state.
   struct Actions {
      int dealer;
      int pone;
   };

   CardsDiscarded get_action(bool dealer,
                             const CardsDealt& cards) const noexcept;

   // Returns the actions for the hand with the given key.
   Actions get_actions(uint64_t key) const noexcept;

   // Returns true if actions have been defined for the given key.
   bool contains(uint64_t key) const noexcept;

   // Output a human-readable representation of the corresponding actions.
   void format_actions(std::ostream& out,
                       const CardsDealt& cards) const noexcept;

   // Load/save the strategy from/to a file.
   bool load(const char* filename) noexcept;
   void save(const char* filename) const noexcept;

   // Add a new actions to the strategy. If an entry already exists for the
   // given key, it is silently overwritten.
   void insert(uint64_t key, int dealer_action, int pone_action);

private:
   using MapType = std::unordered_map<uint64_t, Actions>;
   MapType strategy_;
};

inline PureDiscardStrategy::Actions
PureDiscardStrategy::get_actions(uint64_t key) const noexcept
{
   auto i = strategy_.find(key);
   assert(i != strategy_.end());
   return i->second;
}

inline bool PureDiscardStrategy::contains(uint64_t key) const noexcept
{
   return strategy_.find(key) != strategy_.end();
}

// Generate a greedy discard strategy, i.e., a strategy that maximizes the
// number of guaranteed points.
PureDiscardStrategy generate_greedy_strategy();

// Invokes Fn for every possible combination of cards dealt.
template<typename Fn>
void generate_cards_dealt(Fn fn) noexcept
{
   // It's way easier to hardcode this for a standard Cribbage game than to
   // handle all possibilities.
   static_assert(num_cards_in_deck == 52);
   static_assert(num_cards_dealt_per_player == 6);

   for (auto i0 = 0; i0 < 52; ++i0) {
      for (auto i1 = i0 + 1; i1 < 52; ++i1) {
         for (auto i2 = i1 + 1; i2 < 52; ++i2) {
            for (auto i3 = i2 + 1; i3 < 52; ++i3) {
               for (auto i4 = i3 + 1; i4 < 52; ++i4) {
                  for (auto i5 = i4 + 1; i5 < 52; ++i5) {
                     CardsDealt cards = {
                        Card(i0), Card(i1), Card(i2),
                        Card(i3), Card(i4), Card(i5)
                     };
                     fn(cards);
                 }
               }
            }
         }
      }
   }
}

#endif /* DiscardStrategy_h */
