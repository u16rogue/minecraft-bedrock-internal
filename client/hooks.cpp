#include "hooks.hpp"

#include <Windows.h>
#include <vector>

#include <MinHook.h>

#include <sdk/vec.hpp>

#define __mk_hk(rt, name, ...)                \
  static rt(*name)(__VA_ARGS__) = nullptr;    \
  static auto __hk_##name(__VA_ARGS__) -> rt; \
  static auto __hk_##name(__VA_ARGS__) -> rt

#define __cr_hk(name, addr) \
  MH_CreateHook(addr, reinterpret_cast<void *>(&__hk_##name), reinterpret_cast<void **>(&name))

// ---------------------------------------------------------------------------------------------------- 
// --- Hooks

__mk_hk(void, mc_entt_add_delta, void * self, mcbre::sdk::vec3 * delta) {
  return mc_entt_add_delta(self, delta);
}

// --- End of Hooks
// ---------------------------------------------------------------------------------------------------- 

auto hooks::initialize() -> bool {
  if (MH_Initialize() != MH_OK)
    return false;

  std::uint8_t * base = (decltype(base))GetModuleHandle(NULL);
  __cr_hk(mc_entt_add_delta, base + 0x25E58E0);

  return MH_EnableHook(MH_ALL_HOOKS);
}

#undef __cr_hk
#undef __mk_hk
