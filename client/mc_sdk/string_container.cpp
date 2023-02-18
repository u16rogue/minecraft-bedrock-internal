#include "string_container.hpp"

auto mc::string_container::c_str() -> char * {
  if (length >= sizeof(inplace))
    return ext;
  return inplace;
}

