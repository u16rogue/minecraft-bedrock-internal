#pragma once

#include <cstdint>

#include <common/metapp.hpp>

namespace mcbre::details {

struct pattern_fragment {
  std::uint8_t byte;
  bool         mask;
};

auto pattern_scan(void * base, int sz, const pattern_fragment * pattern, int pattern_count, void ** out) -> bool;

constexpr auto ptype(char p) -> char {
  if (p >= '0' && p <= '9' || p >= 'A' && p <= 'F' || p >= 'a' && p <= 'f')
    return 'N'; // nibble
  else if (p == ' ')
    return 'S'; // space
  else if (p == '?')
    return 'W'; // wildcard

  return 'I';
}

template <int sz> requires (sz > 2)
consteval auto calc_pattern(const char (&pattern)[sz]) -> int { 
  int count      = 0;
  for (int i = 0; i < sz - 1; ++i) {
    auto getoffp = [&](int offset) -> char { return ptype(pattern[i + offset]); };
    bool is_last = i == (sz - 2);
    char c       = pattern[i];

    switch (ptype(c)) {
      case 'N': {
        if (getoffp(1) == 'N') {
          if (pattern[i + 2] != '\0' && getoffp(2) != 'S')
            return -1;
          i += 2;
          ++count;
        } else {
          return -1;
        }
        break;
      }
      case 'W': {
        if (count == 0)
          return -1;
        ++count;
        while (ptype(pattern[i]) == 'W') ++i;
        break;
      }
      case 'S':
        if (count == 0 || is_last)
          return -1;
        break;
      case 'I':
      default:
        return -1;
        break;
    }
  }

  return count;
}

} // mcbre::details

namespace mcbre {

template <metapp::comptime_str str>
struct pattern {
  consteval pattern() {
    // We assume that the pattern is correct as calc_pattern will fail if it wasnt
    constexpr auto cgetv = [](char c) -> std::uint8_t {
      if (c >= '0' && c <= '9')
        return c - '0';
      else if (c >= 'a' && c <= 'f')
        return c - 'a' + 0xa;
      else if (c >= 'A' && c <= 'F')
        return c - 'A' + 0xA;
      return 255;
    };
    int pi = 0; // Pattern index
    auto & pat = str.data();
    // constexpr auto cutenfunny = metapp::array_size(pattern.data);
    for (int i = 0; i < (sizeof(pat) / sizeof(pat[0])); ++i) {
      char c = pat[i];
      auto & frag = frags[pi];
      switch (details::ptype(pat[i])) {
        case 'N': {
          frag.byte = (cgetv(c) << 4) | cgetv(pat[i + 1]);
          frag.mask = true;
          ++pi;
          i += 2;
          break;
        }
        case 'S': {
          break;
        }
        case 'W': {
          frag.byte = 0x00;
          frag.mask = false;
          while (details::ptype(pat[i]) == 'W') ++i;
          ++pi;
          break;
        }
      }
    }
  }

  static constexpr int count = details::calc_pattern(str.data());
  static_assert(count > 0, "Invalid pattern");

  details::pattern_fragment frags[count];
};

template <typename T, int pcount, typename... vargs_t>
auto pattern_scan(void * base, int sz, const details::pattern_fragment (&pattern)[pcount], T & out, vargs_t... offsets) -> bool {
  if (!details::pattern_scan(base, sz, pattern, pcount, reinterpret_cast<void **>(&out))) {
    return false;
  }

  ([&]{
    auto as_u8 = reinterpret_cast<std::uint8_t *&>(out);
    if constexpr (sizeof...(offsets) == 1) {
      as_u8 += offsets;
    } else {
      as_u8 = *reinterpret_cast<std::uint8_t **>(as_u8 + offsets);
    }
  } (), ...);
  return true;
}

} // mcbre
