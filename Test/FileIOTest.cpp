//
// Copyright 2022 Stephen E. Bensley
//
// This file is licensed under the MIT License. You may obtain a copy of the
// license at https://github.com/stephenbensley/Goosey/blob/main/LICENSE.
//

#include "Catch.hpp"
#include "FileIO.h"
#include <map>
#include <vector>

constexpr char filename[] = "test.dat";

TEST_CASE("write/read_pod_map", "[util]")
{
   std::map<int, int> in = {
      { 1,  1},
      { 2,  4},
      { 3,  9},
      { 4, 16},
      { 5, 25}
   };
   std::map<int, int> out;

   {
      std::ofstream ostrm(filename, std::ios::binary | std::ios::trunc);
      write_pod_map(ostrm, in);
   }

   {
      std::ifstream istrm(filename, std::ios::binary);
      REQUIRE(istrm.is_open());
      auto success = read_pod_map(istrm, out);
      REQUIRE(success);
      REQUIRE(read_complete(istrm));
   }

   REQUIRE(in == out);
}

TEST_CASE("write/read_pod_vector", "[util]")
{
   std::vector<int> in = {
      1, 2, 3, 4, 5, 6, 7, 8, 9, 10
   };
   std::vector<int> out;

   {
      std::ofstream ostrm(filename, std::ios::binary | std::ios::trunc);
      write_pod_vector(ostrm, in);
   }

   {
      std::ifstream istrm(filename, std::ios::binary);
      REQUIRE(istrm.is_open());
      auto success = read_pod_vector(istrm, out);
      REQUIRE(success);
      REQUIRE(read_complete(istrm));
   }

   REQUIRE(in == out);
}

