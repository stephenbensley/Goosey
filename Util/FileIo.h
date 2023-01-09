//
// Copyright 2022 Stephen E. Bensley
//
// This file is licensed under the MIT License. You may obtain a copy of the
// license at https://github.com/stephenbensley/Goosey/blob/main/LICENSE.
//

#ifndef FileIo_h
#define FileIo_h

#include <cstdint>
#include <fstream>

// Custom exception for file not found. This exception is not thrown by the
// functions in FileIO, but is defined here for use by callers.
class FileNotFound : public std::exception
{
public:
   virtual const char* what() const noexcept override
   {
      return "Error: file not found.";
   }
};

// All read functions return true on success. If the function fails, any out
// parameters are unmodified.

// Verifies that all the bytes in the file have been read. This should be the
// last call when procressing an input file.
bool read_complete(std::ifstream& istrm) noexcept;

// Write/read "plain ol' data", i.e., data types that are memcpy'able.
template<typename T>
void write_pod(std::ofstream& ostrm, const T& t) noexcept
{
   ostrm.write(reinterpret_cast<const char*>(&t), sizeof(t));
}

template<typename T>
bool read_pod(std::ifstream& istrm, T& t) noexcept
{
   char buf[sizeof(T)];
   if (!istrm.read(buf, sizeof(buf))) {
      return false;
   }
   memcpy(&t, buf, sizeof(T));
   return true;
}

// Write a map (ordered or unordered) where both the key and the mapped type
// are PoD.
template<typename T>
void write_pod_map(std::ofstream& ostrm, const T& t) noexcept
{
   uint64_t entries = t.size();
   write_pod(ostrm, entries);
   for (auto& i : t) {
      write_pod(ostrm, i.first);
      write_pod(ostrm, i.second);
   }
}

// Read a map (ordered or unordered) where both the key and the mapped type
// are PoD.
template<typename T>
bool read_pod_map(std::ifstream& istrm, T& t)
{
   uint64_t entries;
   if (!read_pod(istrm, entries)) {
      return false;
   }
   T tmp;
   while (entries-- > 0) {
      typename T::key_type key;
      if (!read_pod(istrm, key)) {
         return false;
      }
      auto& value = tmp[key];
      if (!read_pod(istrm, value)) {
         return false;
      }
   }
   tmp.swap(t);
   return true;
}

// Write a vector where the elements are PoD.
template<typename T>
void write_pod_vector(std::ofstream& ostrm, const T& t) noexcept
{
   uint64_t entries = t.size();
   write_pod(ostrm, entries);
   auto bytes = sizeof(typename T::value_type) * entries;
   ostrm.write(reinterpret_cast<const char*>(t.data()), bytes);
}

// Read a vector where the elements are PoD.
template<typename T>
bool read_pod_vector(std::ifstream& istrm, T& t)
{
   uint64_t entries;
   if (!read_pod(istrm, entries)) {
      return false;
   }
   T tmp(entries);
   auto bytes = sizeof(typename T::value_type) * entries;
   if (!istrm.read(reinterpret_cast<char*>(tmp.data()), bytes)) {
      return false;
   }
   tmp.swap(t);
   return true;
}

inline bool read_complete(std::ifstream& istrm) noexcept
{
   return istrm.peek() == std::istream::traits_type::eof();
}

#endif /* FileIo_h */
