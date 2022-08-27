#ifndef CardPlayScore_h
#define CardPlayScore_h

#include "Card.h"
#include "RankKeys.h"
#include <array>
#include <cstdint>
#include <unordered_map>

// Creates a map of all possible card series to their corresponding point
// values.
class CardPlayScores
{
public:
   using ScoreMap = std::unordered_map<OrderedRanksKey::KeyType, int>;

   // Creating the map is expensive, so everyone shares a read-only singleton.
   static const ScoreMap& get();

private:
   static constexpr char filename[] = "cardplayscores.dat";

   // Used during initialization to traverse the tree of all possible series.
   struct ScoreNode
   {
      ScoreNode() noexcept;
      // Return false if a card of that rank can't be played.
      bool try_push(int rank) noexcept;
      // Undoes a successful call to try_push.
      void pop() noexcept;

      // Unique key identifying the sequence of cards.
      OrderedRanksKey key;
      // Current count.
      int count = 0;
      // Number of cards of each rank remaining.
      std::array<int, num_card_ranks> remaining;
      // All cards played so far.
      CardsInPlay cards;
   };

   // Constructor is private since this is a singleton.
   CardPlayScores();
   // Called recursively during initialization to traverse the tree.
   void evaluate(ScoreNode& node);
   // Load/save the map state to a file.
   bool load() noexcept;
   void save() const noexcept;

   ScoreMap scores_;
};

// Scores a card series using a cached lookup, rather than starting from
// scratch. Also supports incremental update, which is useful when traversing
// game trees.
class CardPlayScore
{
public:
   void clear() noexcept;
   void update(int rank) noexcept;
   int score() const noexcept;
   int update_and_score(int rank) noexcept;

private:
   OrderedRanksKey key_;
};

inline void CardPlayScore::clear() noexcept
{
   key_.clear();
}

inline void CardPlayScore::update(int rank) noexcept
{
   key_.push_back(rank);
}

inline int CardPlayScore::score() const noexcept
{
   return CardPlayScores::get().find(key_())->second;
}

inline int CardPlayScore::update_and_score(int rank) noexcept
{
   update(rank);
   return score();
}

#endif /* CardPlayScore_h */
