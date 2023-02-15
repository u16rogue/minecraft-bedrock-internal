#include <common/pattern_scanner.hpp>

auto mcbre::details::pattern_scan(void * base, int sz, const pattern_fragment * pattern, int pattern_count, void ** out) -> bool {
  std::uint8_t * c = reinterpret_cast<decltype(c)>(base);
  const std::uint8_t * end = c + sz;

  for (; c < end - pattern_count; ++c) {
    for (int i = 0; i < pattern_count; ++i) {
      auto & frag = pattern[i];
      if (!frag.mask)
        continue;

      if (frag.byte != c[i])
        break;

      if (i == pattern_count - 1) {
        *out = c;
        return true;
      }
    }
  }

  return false;
}

