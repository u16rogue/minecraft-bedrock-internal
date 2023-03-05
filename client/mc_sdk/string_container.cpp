#include "string_container.hpp"

#include "../game.hpp"

auto mc::string_container::construct() -> void {
  return reinterpret_cast<void(*)(string_container *)>(game::mc_string_container_constructor)(this);
}

auto mc::string_container::c_str() -> char * {
  if (length >= sizeof(inplace))
    return ext;
  return inplace;
}

auto mc::string_container::set_str(const char * str, std::size_t sz) -> string_container &
{
  // NOTE: 25/02/2023 - Go do the check somewhere else, if we do it here most likely we'll be doing it at the caller in the first place anyway
  //              wasting time checking if its a valid pointer.
  return *(reinterpret_cast<string_container*(*)(void *, const void *, std::size_t)>(game::mc_string_container_set_method)(this, str, sz));
}

auto mc::string_container::operator=(std::string_view str) -> string_container & {
  return set_str(str.data(), str.length());
}

