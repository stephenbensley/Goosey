#include "InfoSet.h"
#include <limits>

InfoSet::InfoSet(int num_children) noexcept
: sum_values_(num_children)
{
   assert(num_children > 0);
}

Rank InfoSet::get_rank_to_play(const RanksInHand& plays) const noexcept
{
   assert(plays.size() == sum_values_.size());
   return plays[strategy_ >= 0 ? strategy_ : get_best_strategy()];
}

int InfoSet::get_value(const PlayValues& children) noexcept
{
   assert(children.size() == sum_values_.size());

   for (auto i = 0; i < children.size(); ++i) {
      sum_values_[i] += children[i];
   }

   if (strategy_ == -1) {
      return *std::max_element(children.begin(), children.end());
   }

   return children[strategy_];
}

bool InfoSet::update_strategy() noexcept
{
   auto old_strategy = strategy_;
   strategy_ = get_best_strategy();
   std::fill(sum_values_.begin(), sum_values_.end(), 0);
   return strategy_ != old_strategy;
}

int InfoSet::get_best_strategy() const noexcept
{
   auto i = std::max_element(sum_values_.begin(), sum_values_.end());
   return static_cast<int>(std::distance(sum_values_.begin(), i));
}

Rank InfoSets::get_rank_to_play(InfoSetKey::KeyType key,
                                const RanksInHand& plays) const noexcept
{
   auto i = info_sets_.find(key);
   assert(i != info_sets_.end());

   return i->second.get_rank_to_play(plays);
}

int InfoSets::get_value(InfoSetKey::KeyType key,
                        const PlayValues& children) noexcept
{
   auto i = info_sets_.find(key);
   if (i == info_sets_.end()) {
      auto result = info_sets_.emplace(key, children.size());
      assert(result.second);
      i = result.first;
   }

   return i->second.get_value(children);
}

int InfoSets::update_strategy() noexcept
{
   int result = 0;
   for (auto i = info_sets_.begin(); i != info_sets_.end(); ++i) {
      if (i->second.update_strategy()) {
         ++result;
      }
   }
   return result;
}
