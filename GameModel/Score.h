//
// Copyright 2022 Stephen E. Bensley
//
// This file is licensed under the MIT License. You may obtain a copy of the
// license at https://github.com/stephenbensley/Tensity/blob/main/LICENSE.
//

#ifndef Score_h
#define Score_h

// Global functions for computing cribbage scores.

class Card;

// Partial hands are allowed, and starter may equal the nullcard.
int score_hand(const Card* begin,
               const Card* end,
               Card starter,
               bool crib) noexcept;

// Caller may supply the count; otherwise, it will be calculated.
int score_cards_in_play(const Card* begin,
                        const Card* end,
                        int count = -1) noexcept;

#endif /* Score_h */
