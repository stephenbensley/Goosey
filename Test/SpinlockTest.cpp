#include "Catch.hpp"
#include "Spinlock.h"
#include <chrono>
#include <future>
#include <vector>
using namespace std::chrono_literals;

struct SpinlockTestState {
   Spinlock lock;
   std::atomic<int> count = 0;
};

int spinlock_test_worker(SpinlockTestState* test)
{
   // Count the number of times we share access to the state.
   int shared = 0;

   for (auto i = 0; i < 100; ++i) {
      SpinlockGuard guard(test->lock);
      shared += test->count.fetch_add(1);
      std::this_thread::sleep_for(1ms);
      --(test->count);
   }

   return shared;
}

TEST_CASE("Spinlock", "[util]")
{
   auto concurrency = std::thread::hardware_concurrency();
   SpinlockTestState state;

   // Launch the workers ...
   std::vector<std::future<int>> futures;
   for (auto i = 0; i < concurrency; ++i) {
      futures.push_back(std::async(std::launch::async,
                                   &spinlock_test_worker,
                                   &state));
   }
   // ... and wait for them to complete.
   auto shared = std::reduce(futures.begin(),
                             futures.end(),
                             0,
                             [](auto lhs, auto& rhs) {
      return lhs + rhs.get();
   });

   // If the lock is working, the state should never be shared.
   REQUIRE(shared == 0);
}
