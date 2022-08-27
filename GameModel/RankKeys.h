#ifndef RankKeys_h
#define RankKeys_h

#include "Card.h"

// Generates an integer key that uniquely identifies an ordered set of ranks.
class OrderedRanksKey
{
public:
   using KeyType = uint32_t;

   KeyType operator()() const noexcept;
   bool empty() const noexcept;
   void clear() noexcept;
   void push_back(Rank rank) noexcept;
   void pop_back() noexcept;

private:
   KeyType key_ = 0;

   // Number of bits needed to represent a rank.
   static constexpr int bit_width = 4;
   // Make sure we have enough bits for all ranks.
   static_assert(num_card_ranks < (1u << bit_width));
   // Make sure we hae enough bits for all cards.
   static_assert((bit_width * max_cards_in_play) <= (sizeof(KeyType) * 8));
};

inline OrderedRanksKey::KeyType OrderedRanksKey::operator()() const noexcept
{
   return key_;
}

inline bool OrderedRanksKey::empty() const noexcept
{
   return key_ == 0;
}

inline void OrderedRanksKey::clear() noexcept
{
   key_ = 0;
}

inline void OrderedRanksKey::push_back(Rank rank) noexcept
{
   key_ <<= bit_width;
   key_ |= rank;
}

inline void OrderedRanksKey::pop_back() noexcept
{
   key_ >>= bit_width;
}

// Generates an integer key that uniquely identifies an unordered set of ranks.
class UnorderedRanksKey
{
public:
   using KeyType = uint32_t;

   KeyType operator()() const noexcept;
   bool empty() const noexcept;
   void clear() noexcept;
   void insert(Rank rank) noexcept;
   void erase(Rank rank) noexcept;

private:
   KeyType key_ = 0;

   static KeyType multiplier(Rank rank) noexcept;
};

inline UnorderedRanksKey::KeyType UnorderedRanksKey::operator()() const noexcept
{
   return key_;
}

inline bool UnorderedRanksKey::empty() const noexcept
{
   return key_ == 0;
}

inline void UnorderedRanksKey::clear() noexcept
{
   key_ = 0;
}

inline void UnorderedRanksKey::insert(Rank rank) noexcept
{
   key_ += multiplier(rank);
}

inline void UnorderedRanksKey::erase(Rank rank) noexcept
{
   key_ -= multiplier(rank);
}

inline
UnorderedRanksKey::KeyType UnorderedRanksKey::multiplier(Rank rank) noexcept
{
   // We use base-5 to store numbers from 0...4
   static_assert(num_card_suits == 4);
   static constexpr std::array<KeyType, num_card_ranks> multipliers = {
      1, 5, 25, 125, 625, 3125, 15625, 78125, 390625,
      1953125, 9765625, 48828125, 244140625
   };

   return multipliers[rank_ordinal(rank)];
}

#endif /* RankKeys_h */
