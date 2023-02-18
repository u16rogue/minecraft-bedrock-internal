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
  char unk[8];

  auto c_str() -> char *;
  // auto operator=(std::string_view str) -> void;
};

/*
static_assert(
    offsetof(mc::string_container, ext) == 0 && offsetof(mc::string_container, inplace) == 0
 && offsetof(mc::string_container, length) == 0x10
 && sizeof(mc::string_container) == 0x20
, "mc::string_container is invalid.");
*/

}
