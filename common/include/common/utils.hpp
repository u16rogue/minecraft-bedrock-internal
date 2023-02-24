#pragma once

#include <cstdint>
#include <tuple>
#include <cstddef>

namespace mcbre::utils {

template <typename T, typename... vargs_t>
auto walk_offsets(T *& out, void * address, vargs_t... offsets) -> bool {
  std::uint8_t * cur = reinterpret_cast<decltype(cur)>(address);
  
  int i = sizeof...(offsets);
  for (auto offset : { static_cast<std::int64_t>(offsets)... }) {
    --i;
    if (i) {
      cur = *reinterpret_cast<std::uint8_t **>(cur + offset);
    } else {
      cur += offset;
    }

    if (!cur)
      return false;
  }

  if (cur && i == 0) {
    out = reinterpret_cast<T *>(cur);
    return true;
  }

  return false;
}

auto module_info(const char * name) -> std::pair<std::uint8_t *, std::size_t>;

} // mcbre::utils
