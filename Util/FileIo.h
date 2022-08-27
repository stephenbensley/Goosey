#ifndef FileIo_h
#define FileIo_h

#include <cstdint>
#include <fstream>

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
bool read_pod_map(std::ifstream& istrm, T& t) noexcept
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
bool read_pod_vector(std::ifstream& istrm, T& t) noexcept
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
