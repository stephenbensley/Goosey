//
// Copyright 2022 Stephen E. Bensley
//
// This file is licensed under the MIT License. You may obtain a copy of the
// license at https://github.com/stephenbensley/Goosey/blob/main/LICENSE.
//

#include "Catch.hpp"
#include "SizedArray.h"

TEST_CASE("SizedArray::erase", "[util]")
{
   std::array<int, 10> values = { 1, 4, 5, 4, 6, 8, 9, 10, 5, 4 };
   SizedArray<int, 12> sized;
   sized.insert(values.begin(), values.end());
   REQUIRE(sized.size() == values.size());

   REQUIRE(sized.erase(5) == 2);
   REQUIRE(sized.size() == 8);

   std::array<int, 3> remove = { 4, 15, 9 };
   REQUIRE(sized.erase(remove.begin(), remove.end()) == 4);
   REQUIRE(sized.size() == 4);

   REQUIRE(sized.erase(12) == 0);

   std::array<int, 4> remaining;
   std::copy(sized.begin(), sized.end(), remaining.begin());
   std::array<int, 4> expected = { 1, 6, 8, 10};

   REQUIRE(remaining == expected);
}
