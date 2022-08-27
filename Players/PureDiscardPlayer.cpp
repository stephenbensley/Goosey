#include "PureDiscardPlayer.h"
#include "Canonize.h"
#include "CardSplitter.h"

PureDiscardPlayer::PureDiscardPlayer() noexcept
{
   strategy_.load(filename);
}

std::unique_ptr<Player> PureDiscardPlayer::clone() const
{
    return std::make_unique<PureDiscardPlayer>(*this);
}

CardsDiscarded PureDiscardPlayer::get_discards(const GameView& game,
                                               const CardsInHand& hand)
{
   CardsDealt canonical(hand.data());
   auto actions = strategy_.find(canonize(canonical));
   CardSplitter splitter(canonical);
   splitter.seek(is_dealer(game) ? actions.dealer : actions.pone);
   return splitter.crib;
}
