#include "Deck.h"
#include <algorithm>
#include <random>

Deck::Deck() noexcept
: rng_(pcg_extras::seed_seq_from<std::random_device>())
{
   cards_.resize(num_cards_in_deck);
   std::generate(cards_.begin(),
                 cards_.end(),
                 [i = 0]() mutable { return Card(i++); });
   next_ = cards_.end();
}

Deck::Deck(const Deck& other) noexcept
: cards_(other.cards_),
  next_(cards_.begin() + other.size()),
  rng_(other.rng_)
{ }

Deck& Deck::operator=(const Deck& rhs) noexcept
{
   if (this != &rhs) {
      cards_ = rhs.cards_;
      next_ = cards_.begin() + rhs.size();
      rng_ = rhs.rng_;
   }
   return *this;
}

void Deck::shuffle(int num_cards) noexcept
{
   assert(num_cards < cards_.size());

   // No need to shuffle the entire deck.
   next_ = cards_.begin() + num_cards;

   auto num_left = static_cast<int>(cards_.size());
   for (auto i = cards_.begin(); i != next_; ++i) {
      std::iter_swap(i, (i + rng_(num_left--)));
   }
}

CardsDealt deal_cards(Deck& deck) noexcept
{
   CardsDealt cards;
   std::generate(cards.begin(),
                 cards.end(),
                 [&deck](){ return deck.deal_card(); });
   return cards;
}

