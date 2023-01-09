//
// Copyright 2022 Stephen E. Bensley
//
// This file is licensed under the MIT License. You may obtain a copy of the
// license at https://github.com/stephenbensley/Goosey/blob/main/LICENSE.
//

#ifndef CribDefs_h
#define CribDefs_h

// Various constants that define the game of cribbage. Theoretically, all of
// the non-derived values can be changed, but only the values used for a
// standard game of two-player cribbage have been well-tested.

constexpr int num_players = 2;

constexpr int num_card_suits = 4;
constexpr int num_card_ranks = 13;
constexpr int num_cards_in_deck = num_card_suits * num_card_ranks;
constexpr int min_card_suit = 1;
constexpr int max_card_suit = min_card_suit + num_card_suits - 1;
constexpr int min_card_rank = 1;
constexpr int max_card_rank = min_card_rank + num_card_ranks - 1;
constexpr int max_card_value = 10;
constexpr int card_rank_of_jack = 11;

constexpr int num_cards_dealt_per_player = 6;
constexpr int num_cards_dealt_to_crib = 0;
constexpr int num_cards_dealt_per_round =
   (num_cards_dealt_per_player * num_players) + num_cards_dealt_to_crib + 1;
constexpr int num_cards_discarded_per_player = 2;
constexpr int num_cards_in_hand =
   num_cards_dealt_per_player - num_cards_discarded_per_player;
constexpr int num_cards_in_crib =
   num_cards_dealt_to_crib + (num_cards_discarded_per_player * num_players);
constexpr int max_cards_in_play = num_cards_in_hand * num_players;

// For all common versions of Cribbage, the hands and the crib are the same
// size. It simplifies the code somewhat if we make this assumption everywhere.
static_assert(num_cards_in_hand == num_cards_in_crib);
constexpr int num_cards_in_show = num_cards_in_hand;

constexpr int max_count_in_play = 31;

constexpr int num_points_for_his_heels = 2;
constexpr int num_points_for_15 = 2;
constexpr int num_points_for_31 = 2;
constexpr int num_points_for_last_card = 1;
constexpr int num_points_for_his_nob = 1;
constexpr int num_points_to_win = 121;

#endif /* CribDefs_h */
