#include "HandVsHand.h"
#include "CardPlayHands.h"
#include "MinimaxStrategy.h"
#include "FileIO.h"
#include <future>

HandVsHand::HandVsHand()
: table_(std::make_unique<Table>())
{
   auto& hands = CardPlayHands::get().hands(num_cards_in_hand);
   assert(hands.size() == num_hands);

   for (auto i = 0; i < num_hands; ++i) {
      index_[key(hands[i].hand)] = i;
   }
}

void HandVsHand::build()
{
   auto num_workers = std::thread::hardware_concurrency();

   // Launch the workers ...
   std::vector<std::future<void>> futures;
   for (auto i = 0; i < num_workers; ++i) {
      futures.push_back(std::async(std::launch::async,
                                   &HandVsHand::build_worker,
                                   this,
                                   i,
                                   num_workers));
   }
   // ... and wait for them to complete.
   std::for_each(futures.begin(), futures.end(), [](auto& f){ f.get(); });
}

bool HandVsHand::load(const char* filename) noexcept
{
   std::ifstream istrm(filename, std::ios::binary);
   if (!istrm.is_open()) {
      return false;
   }
   // Can't use read_pod since it uses a stack-allocated buffer.
   if (!istrm.read(reinterpret_cast<char*>(table_.get()), sizeof(*table_))) {
      return false;
   }
   if (!read_complete(istrm)) {
      return false;
   }
   return true;
}

void HandVsHand::save(const char* filename) const noexcept
{
   std::ofstream ostrm(filename, std::ios::binary | std::ios::trunc);
   write_pod(ostrm, *table_);
}

void HandVsHand::build_worker(int idx, int num_workers) noexcept
{
   auto& hands = CardPlayHands::get().hands(num_cards_in_hand);

   // We divide the dealer hands among the workers.
   for (auto i = idx; i < hands.size(); i += num_workers) {
      auto& dealer = hands[i];
      CardPlayHandsIterator pone(num_cards_in_hand, dealer.counts);
      while (pone.next()) {
         auto points = play_round(dealer.hand, pone.hand());

         auto& cell = (*this)[i][pone.pos()];
         cell.dealer_points = points.first;
         cell.pone_points = points.second;
         cell.dealer_combos = pone.rcombos();
         cell.pone_combos = pone.combos();
      }
   }
}

std::pair<int, int> HandVsHand::play_round(const RanksInHand& dealer,
                                           const RanksInHand& pone) noexcept
{
   static_assert(num_players == 2);

   std::array<MinimaxStrategy, num_players> players;
   players[0].start_new_round(dealer, true);
   players[1].start_new_round(pone, false);

   CardPlayModel model(0);
   std::array<int, num_players> points{};

   do {
      auto player = model.current_player();
      auto play = players[player].get_rank_to_play();
      points[player] += model.play_rank(play);;
      players[other_player(player)].on_opponent_play(play);
   } while (!model.round_over());

   return { points[0], points[1] };
}

HandVsHand::KeyType HandVsHand::key(const CardsKept& hand) noexcept
{
   UnorderedRanksKey key;
   std::for_each(hand.begin(), hand.end(), [&key](auto card) {
      key.insert(card.rank());
   });
   return key();
}

HandVsHand::KeyType HandVsHand::key(const RanksInHand& hand) noexcept
{
   UnorderedRanksKey key;
   std::for_each(hand.begin(), hand.end(), [&key](auto rank) {
      key.insert(rank);
   });
   return key();
}
