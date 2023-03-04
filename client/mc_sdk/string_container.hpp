#pragma once

#include <string_view>
#include <cstdint>

namespace mc {

struct string_container {
  union {
    char * ext;
    char inplace[0x10];
  };
  std::int64_t length;
  std::int64_t unk;

  auto c_str() -> char *;

  auto set_str(const char * str, std::size_t sz) -> string_container &;
  
  template <int sz>
  auto operator=(const char (&str)[sz]) -> string_container & {
    return set_str(str, sz - 1);
  }
};

/*
static_assert(
    offsetof(mc::string_container, ext) == 0 && offsetof(mc::string_container, inplace) == 0
 && offsetof(mc::string_container, length) == 0x10
 && sizeof(mc::string_container) == 0x20
, "mc::string_container is invalid.");
*/

}
