#include "string_container.hpp"

#include "../values.hpp"

auto mc::string_container::c_str() -> char * {
  if (length >= sizeof(inplace))
    return ext;
  return inplace;
}

auto mc::string_container::set_str(const char * str, std::size_t sz) -> string_container &
{
  if (values::mc_string_container_set_method) {
    return *(reinterpret_cast<string_container*(*)(void *, const void *, std::size_t)>(values::mc_string_container_set_method)(this, str, sz));
  }
  return *this;
}
