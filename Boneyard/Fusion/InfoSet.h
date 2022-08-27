#ifndef InfoSetKey_h
#define InfoSetKey_h

#include "CardPlayDefs.h"
#include <unordered_map>

// Integer key that uniquely identifies an information set.
class InfoSetKey
{
public:
   using KeyType = uint32_t;

   KeyType operator()() const noexcept;

   // Reset the key to the root information set.
   void clear() noexcept;

   // Update the key based on the given play.
   void make_play(Rank play, bool observer) noexcept;

private:
   // 4 nibbles with the observer's plays.
   uint16_t lo_ = 0;
   // 4 nibbles with the opponent's plays.
   uint16_t hi_ = 0;

   // Number of bits needed to represent a rank.
   static constexpr int bit_width = 4;
   // Make sure we have enough bits for all ranks.
   static_assert(max_card_rank < (1u << bit_width));
   // Make sure we have enough bits for all cards in the hand.
   static_assert((bit_width * num_cards_in_hand) <= (sizeof(uint16_t) * 8));
   // Only support two player games.
   static_assert(num_players == 2);
};

inline InfoSetKey::KeyType InfoSetKey::operator()() const noexcept
{
   return (static_cast<KeyType>(hi_) << 16) | lo_;
}

inline void InfoSetKey::clear() noexcept
{
   lo_ = hi_ = 0;
}

inline void InfoSetKey::make_play(Rank rank, bool observer) noexcept
{
   if (observer) {
      lo_ <<= bit_width;
      lo_ += rank;
   } else {
      hi_ <<= bit_width;
      hi_ += rank;
   }
}

using PlayValues = SizedArray<int, num_cards_in_hand>;

// Tracks state for an information set.
class InfoSet
{
public:
   InfoSet(int num_children) noexcept;

   Rank get_rank_to_play(const RanksInHand& plays) const noexcept;

   // Returns the value of the current play for this infoset.
   int get_value(const PlayValues& children) noexcept;

   // Updates the infoset strategy; returns 'true' if the strategy changed.
   bool update_strategy() noexcept;

   int size() const noexcept;

private:
   // Which play to make at this infoset. If strategy == -1, the node follows
   // minimax instead of playing a fixed strategy.
   int strategy_ = -1;
   // Accumulated values of making each play.
   PlayValues sum_values_;

   int get_best_strategy() const noexcept;
};

inline int InfoSet::size() const noexcept
{
   return static_cast<int>(sum_values_.size());
}

class InfoSets
{
public:

   Rank get_rank_to_play(InfoSetKey::KeyType key,
                         const RanksInHand& plays) const noexcept;
   // Returns the value of the current play for the given infoset.
   int get_value(InfoSetKey::KeyType key, const PlayValues& children) noexcept;

   // Updates the strategy for all info sets; returns the number of nodes whose
   // strategy changed.
   int update_strategy() noexcept;


   void clear() noexcept;

private:
   std::unordered_map<InfoSetKey::KeyType, InfoSet> info_sets_;
};

inline void InfoSets::clear() noexcept
{
   info_sets_.clear();
}

#endif /* InfoSetKey_h */
