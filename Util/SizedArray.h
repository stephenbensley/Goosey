//
// Copyright 2022 Stephen E. Bensley
//
// This file is licensed under the MIT License. You may obtain a copy of the
// license at https://github.com/stephenbensley/Goosey/blob/main/LICENSE.
//

#ifndef SizedArray_h
#define SizedArray_h

#include <algorithm>
#include <array>
#include <cassert>
#include <iterator>

// Represents an array where not all values are necessarily present. Similar to
// a vector with fixed capacity.
template<typename T, int N>
class SizedArray
{
public:
   using size_type = size_t;
   using reference = T&;
   using const_reference = const T&;
   using iterator = T*;
   using const_iterator = const T*;

   SizedArray() noexcept = default;

   explicit SizedArray(size_type count, const T& value = T())
   {
      resize(count);
      std::fill(begin(), end(), value);
   }

   SizedArray(const SizedArray& other) noexcept
   : data_(other.data_)
   {
      end_ = begin() + other.size();
   }

   SizedArray& operator=(const SizedArray& rhs) noexcept
   {
      if (this != &rhs) {
         data_ = rhs.data_;
         end_ = begin() + rhs.size();
      }
      return *this;
   }

   reference operator[](size_type pos) noexcept
   {
      assert(pos < size());
      return data_[pos];
   }

   const_reference operator[](size_type pos) const noexcept
   {
      assert(pos < size());
      return data_[pos];
   }

   reference front() noexcept
   {
      assert(!empty());
      return data_.front();
   }

  const_reference front() const noexcept
   {
      assert(!empty());
      return data_.front();
   }

   reference back() noexcept
   {
      assert(!empty());
      return *(end_ - 1);
   }

   const_reference back() const noexcept
   {
      assert(!empty());
      return *(end_ - 1);
   }

   const std::array<T, N>& data() const noexcept
   {
      assert(size() == N);
      return data_;
   }
   
   iterator begin() noexcept
   {
      return data_.begin();
   }

   const_iterator begin() const noexcept
   {
      return data_.begin();
   }

   iterator end() noexcept
   {
      return end_;
   }

   const_iterator end() const noexcept
   {
      return end_;
   }

   bool contains(const_reference value) const noexcept
   {
      return std::find(begin(), end(), value) != end();
   }

   bool empty() const noexcept
   {
      return begin() == end();
   }

   size_type size() const noexcept
   {
      return std::distance(begin(), end());
   }

   size_type capacity() const noexcept
   {
      return N;
   }

   void clear() noexcept
   {
      end_ = begin();
   }
   
   void push_back(const_reference value) noexcept
   {
      assert(size() < N);
      *end_++ = value;
   }

   void pop_back() noexcept
   {
      assert(!empty());
      --end_;
   }

   size_type erase(const_reference value) noexcept
   {
      auto old_end = end_;
      end_ = std::remove(begin(), end(), value);
      return std::distance(end_, old_end);
   }

   size_type erase(const_iterator begin, const_iterator end) noexcept
   {
      auto old_end = end_;
      end_ = std::remove_if(this->begin(),
                            this->end(),
                            [begin, end](const auto& value) {
         return std::find(begin, end, value) != end;
      });
      return std::distance(end_, old_end);
   }

   void insert(const_reference value) noexcept
   {
      push_back(value);
   }

   void insert(const_iterator begin, const_iterator end) noexcept
   {
      std::for_each(begin, end, [this](const auto& c){ insert(c); });
   }

   bool remove_first(const_reference value) noexcept
   {
      auto i = std::find(begin(), end(), value);
      if (i == end()) {
         return false;
      }
      std::iter_swap(i, std::prev(end()));
      pop_back();
      return true;
   }

   void resize(size_type new_size) noexcept
   {
      end_ = data_.begin() + new_size;
   }

private:
   std::array<T, N> data_;
   T* end_ = data_.begin();
};

#endif /* SizedArray_h */
