#ifndef CardSet_h
#define CardSet_h

#include "Card.h"
#include <array>
#include <bitset>

// Maintains a set of flags for each suit indicating which ranks are present.
class CardSet
{
public:
   CardSet(const Card* begin, const Card* end) noexcept;

   // Returns the suit flags for a zero-based index.
   uint16_t operator[](int index) const noexcept;
   // Returns the suit flags for the suit of the card.
   uint16_t operator[](Card c) const noexcept;
   // Returns the number of cards present of the same suit as the card.
   int count(Card c) const noexcept;
   // Sorts the suit flags.
   void sort() noexcept;

private:
   using FlagsType = std::bitset<num_card_ranks>;
   // We're assuming the flags can fit in a uint16_t
   static_assert(num_card_ranks <= 16);

   FlagsType& at(int suit) noexcept;
   const FlagsType& at(int suit) const noexcept;

   std::array<FlagsType, num_card_suits> flags_;
};

inline CardSet::CardSet(const Card* begin, const Card* end) noexcept
{
   std::for_each(begin, end, [this](auto c) {
      at(c.suit()).set(c.rank() - min_card_rank);
   });
}

inline uint16_t CardSet::operator[](int index) const noexcept
{
   assert(index < flags_.size());
   return flags_[index].to_ulong();
}

inline uint16_t CardSet::operator[](Card c) const noexcept
{
   return at(c.suit()).to_ulong();
}

inline int CardSet::count(Card c) const noexcept
{
   return static_cast<int>(at(c.suit()).count());
}

inline void CardSet::sort() noexcept
{
   std::sort(flags_.begin(), flags_.end(), [](auto lhs, auto rhs) {
      return lhs.to_ulong() < rhs.to_ulong();
   });
}

inline CardSet::FlagsType& CardSet::at(int suit) noexcept
{
   assert(suit >= min_card_suit);
   assert(suit <= max_card_suit);
   return flags_[suit - min_card_suit];
}

inline const CardSet::FlagsType& CardSet::at(int suit) const noexcept
{
   assert(suit >= min_card_suit);
   assert(suit <= max_card_suit);
   return flags_[suit - min_card_suit];
}

#endif /* CardSet_h */
