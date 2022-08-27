#include <algorithm>
#include <charconv>
#include <iostream>
#include <string>

int discard_main(int argc, char* const argv[]);
int hvh_main(int argc, char* const argv[]);
int match_main(int argc, char* const argv[]);

int show_usage() noexcept
{
   std::cout << "usage: cribbage <verb> ..." << std::endl
             << "Valid verbs:" << std::endl
             << "   discard" << std::endl
             << "   hvh" << std::endl
             << "   match" << std::endl;

   return -1;
}

int main(int argc, char* const argv[])
{
   if (argc > 1) {
      std::string lower(argv[1]);
      std::transform(lower.begin(), lower.end(), lower.begin(), tolower);

      if (lower == "discard") {
         return discard_main((argc - 1), (argv + 1));
      }
      if (lower == "hvh") {
         return hvh_main((argc - 1), (argv + 1));
      }
      if (lower == "match") {
         return match_main((argc - 1), (argv + 1));
      }
   }
   return show_usage();
}
