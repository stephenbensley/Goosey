#ifndef Spinlock_h
#define Spinlock_h

#include <atomic>

// Implements a lightweight spinlock. Suitable for serializing access under
// low contention.
class Spinlock
{
public:
   void lock() noexcept;
   void unlock() noexcept;

private:
   std::atomic<bool> lock_{false};
};

inline void Spinlock::lock() noexcept
{
   while (lock_.exchange(true, std::memory_order_acquire)) {
      while (lock_.load(std::memory_order_relaxed)) { }
   }
}

inline void Spinlock::unlock() noexcept
{
   lock_.store(false, std::memory_order_release);
}

// RAII guard class for a Spinlock.
class SpinlockGuard
{
public:
   explicit SpinlockGuard(Spinlock& lock) noexcept;
   SpinlockGuard(const SpinlockGuard&) = delete;
   SpinlockGuard& operator=(const SpinlockGuard&) = delete;

   ~SpinlockGuard() noexcept;

private:
   Spinlock& lock_;
};

inline SpinlockGuard::SpinlockGuard(Spinlock& lock) noexcept
: lock_(lock)
{
   lock_.lock();
}

inline SpinlockGuard::~SpinlockGuard() noexcept
{
   lock_.unlock();
}

#endif /* Spinlock_h */
