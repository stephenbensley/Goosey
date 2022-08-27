#include "DiscardBestResponse.h"
#include <iostream>

int discard_main(int argc, char* const argv[])
{
   PureDiscardStrategy strategy;
   auto success = strategy.load("discard.dat");
   if (!success) {
      std::cout << "Failed to load response10.dat" << std::endl;
      return -1;
   }

   HandVsHand hvh;
   success = hvh.load("hvh.dat");
   if (!success) {
      std::cout << "Failed to load hvh.dat" << std::endl;
      return -1;
   }

   for (auto i = 0; i < 99; ++i) {
      DiscardBestResponse response(strategy, hvh);

      char filename[20];
      std::sprintf(filename, "response%02d.dat", i);

      std::cout << "Computing " << filename << std::endl;
      response.compute(10'000'000'000);

      std::cout << "Exploitablity: " << response.exploitability() << std::endl;
      strategy = response.best_response();
      strategy.save(filename);
   }

   return 0;
}
