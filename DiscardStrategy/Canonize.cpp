#include "Canonize.h"
#include "CardSet.h"
#include <algorithm>
#include <tuple>

// Packs four uint16_t's into a uint64_t.
uint64_t pack(uint16_t w0, uint16_t w1, uint16_t w2, uint16_t w3) noexcept
{
   uint64_t result = w0;
   result |= static_cast<uint64_t>(w1) << 16;
   result |= static_cast<uint64_t>(w2) << 32;
   result |= static_cast<uint64_t>(w3) << 48;
   return result;
}

uint64_t canonize(CardsDealt& cards) noexcept
{
   // Compute the suit equivalences.
   CardSet equiv(cards.begin(), cards.end());

   // Canonize the input cards.
   std::sort(cards.begin(), cards.end(), [&equiv](auto lhs, auto rhs) {
      return std::make_tuple(equiv[lhs], lhs.suit(), lhs.rank()) <
             std::make_tuple(equiv[rhs], rhs.suit(), rhs.rank());
   });

   // Sort the suit flags.
   equiv.sort();

   // And convert them to a 64-bit integer for convenience.
   return pack(equiv[0], equiv[1], equiv[2], equiv[3]);
}

