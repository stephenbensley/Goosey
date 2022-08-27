#include "Card.h"
#include <sstream>

// Magic string representing a nullcard.
constexpr char nullcard_sz[] = "00";
// Single character symbols used to represent ace through king.
constexpr char rank_symbols[] = "0A23456789XJQK";
// Single character symbols for the suits: spades, hearts, diamonds, clubs.
constexpr char suit_symbols[] = "0SHDC";

// Finds the offset of a symbol in a symbol string. Returns true if found.
bool symbol_to_value(std::string_view symbols, char symbol, char& value)
{
   auto match = std::find(symbols.begin(),
                          symbols.end(),
                          std::toupper(symbol));
   if (match == symbols.end()) { return false; }
   value = static_cast<char>(std::distance(symbols.begin(), match));
   return true;
}

std::string to_string(Rank r)
{
   return { rank_symbols[r] };
}

std::string to_string(const Rank* begin, const Rank* end)
{
   std::string result;
   for (auto r = begin; r != end; ++r) {
      result += to_string(*r);
   }
   return result;
}

bool from_string(std::string_view sv, Card& card) noexcept
{
   // Card strings are exactly two characters long: one for rank, one for suit.
   if (sv.length() != 2) {
      return false;
   }

   if (sv == nullcard_sz) {
      card = nullcard;
      return true;
   }

   Rank rank;
   if (!symbol_to_value(rank_symbols, sv[0], rank)) {
      return false;
   }

   Suit suit;
   if (!symbol_to_value(suit_symbols, sv[1], suit)) {
      return false;
   }

   card = { rank, suit };
   return true;
}

bool from_string(std::string_view sv, std::vector<Card>& cards)
{
   std::string s(sv);
   std::istringstream is(s);
   auto tokens = std::vector(std::istream_iterator<std::string>(is),
                             std::istream_iterator<std::string>());

   std::vector<Card> result;
   for (auto t : tokens) {
      Card card;
      if (!from_string(t, card)) {
         return false;
      }
      result.push_back(card);
   }
   cards.swap(result);
   return true;
}

std::string to_string(Card c)
{
   return { rank_symbols[c.rank()],
            suit_symbols[c.suit()] };
}

std::string to_string(const Card* begin, const Card* end)
{
   std::string result;
   for (auto c = begin; c != end; ++c) {
      // If this isn't the first card, add a space delimiter.
      if (c != begin) {
         result += ' ';
      }
      result += to_string(*c);
   }
   return result;
}

