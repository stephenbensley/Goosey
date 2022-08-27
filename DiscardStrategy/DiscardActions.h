#ifndef Actions_h
#define Actions_h

#include "Card.h"
#include "DiscardDefs.h"
#include <bitset>

// Available discard actions for a given set of dealt cards.
class DiscardActions
{
public:
   DiscardActions() = default;
   // Initialize with the set of actions that are strategically distinct for
   // the set of dealt cards.
   explicit DiscardActions(const CardsDealt& cards) noexcept;

   // Returns the number of available actions.
   int count() const noexcept;
   // Returns true if the specified action is available.
   bool test(int i) const noexcept;

   // Iterate over all available actions.
   template<class Fn>
   void for_each(Fn f) const;

private:
   std::bitset<num_discard_actions> actions_;
};

inline int DiscardActions::count() const noexcept
{
   return static_cast<int>(actions_.count());
}

inline bool DiscardActions::test(int i) const noexcept
{
   return actions_.test(i);
}

template<class Fn>
void DiscardActions::for_each(Fn f) const
{
   for (auto i = 0; i < actions_.size(); ++i) {
      if (actions_.test(i)) {
         f(i);
      }
   }
}

#endif /* Actions_h */
