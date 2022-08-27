#ifndef Canonize_h
#define Canonize_h

#include "Card.h"
#include <cstdint>

// Arranges the cards in canonical order and returns a key that uniquely
// identifies the equivalence class for the hand.
uint64_t canonize(CardsDealt& cards) noexcept;

#endif /* Canonize_h */
