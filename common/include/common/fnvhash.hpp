#pragma once

#include <cstdint>
#include <string_view>

namespace mcbre::hash {

constexpr auto fnv32(const char * data, int sz) -> std::uint32_t {
  using i = std::uint32_t;

  i hash = 0x811c9dc5;
  for (int i = 0; i < sz; ++i) {
    hash = (hash ^ reinterpret_cast<const std::uint8_t *>(data)[i]) * 0x01000193;
  }

  return hash;
}

constexpr auto fnv32(std::string_view str) -> std::uint32_t {
  return mcbre::hash::fnv32(str.data(), str.length());
}

template <int sz> 
consteval auto cvfnv32(const char (&str)[sz]) -> std::uint32_t {
  return mcbre::hash::fnv32(str, sz - 1);
}

}
