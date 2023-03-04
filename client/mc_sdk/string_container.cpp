#include "string_container.hpp"

#include "../values.hpp"

auto mc::string_container::c_str() -> char * {
  // 03/03/2023 - not sure if this is correct seems like its done for commands and stuff
  if (unk >= 0x10)
    return reinterpret_cast<string_container *>(ext)->c_str();

  if (length >= sizeof(inplace))
    return ext;
  return inplace;
}

auto mc::string_container::set_str(const char * str, std::size_t sz) -> string_container &
{
  // NOTE: 25/02/2023 - Go do the check somewhere else, if we do it here most likely we'll be doing it at the caller in the first place anyway
  //              wasting time checking if its a valid pointer.
  return *(reinterpret_cast<string_container*(*)(void *, const void *, std::size_t)>(values::mc_string_container_set_method)(this, str, sz));
}
