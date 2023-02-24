#pragma once

#include <cstdint>
#include <optional>

namespace hooks {

struct hook_entry {
  union {
    void*(*lookup)(void * base, int sz);
    void * cached_target;
  };
  const char * modname;
  void * pout;
  void * hkfn;
  const std::uint32_t id;
  bool hooked = false;

#if defined(MCBRE_STORE_HOOK_NAME) 
  const char * idstr;
#endif
};

auto hook_info_get(std::uint32_t id) -> std::optional<const hooks::hook_entry>;

auto initialize() -> bool;
auto uninitialize() -> bool;

}
