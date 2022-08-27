#ifndef DiscardDefs_h
#define DiscardDefs_h

constexpr int factorial(int n)
{
   return (n <= 1) ? 1 : (n * factorial(n - 1));
}

// Number of different ways a player can choose to discard
// C(n, k) = n!/((n - k)! * k!)
constexpr int num_discard_actions = factorial(num_cards_dealt_per_player) /
                                    (factorial(num_cards_in_hand) *
                                     factorial(num_cards_discarded_per_player));

#endif /* DiscardDefs_h */
