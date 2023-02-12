#include "hooks.hpp"

#include <Windows.h>
#include <d3d12.h>

#include <vector>

#include <MinHook.h>

#include <sdk/vec.hpp>
#include "mc_sdk/entt.hpp"

#define __mk_hk(rt, name, ...)                \
  static rt(*name)(__VA_ARGS__) = nullptr;    \
  static auto __hk_##name(__VA_ARGS__) -> rt; \
  static auto __hk_##name(__VA_ARGS__) -> rt

#define __cr_hk(name, addr) \
  (MH_CreateHook(addr, reinterpret_cast<void *>(&__hk_##name), reinterpret_cast<void **>(&name)) == MH_OK)

// ---------------------------------------------------------------------------------------------------- 
// --- Hooks

__mk_hk(void, mc_entt_add_delta, mc::entt * self, mcbre::sdk::vec3 * delta) {
  static bool funny = false;
  if (GetAsyncKeyState(VK_SPACE) & 0x1) {
    funny = !funny;
  }

  if (funny)
  {
    self->pcoords->x = 0.f;
    self->pcoords->z = 0.f;
    return;
  }

  return mc_entt_add_delta(self, delta);
}

__mk_hk(HRESULT, dx_present, void * self, UINT SyncInterval, UINT Flags) {
  return dx_present(self, SyncInterval, Flags);
}

__mk_hk(HRESULT, dx_present1, void * self, UINT SyncInterval, UINT PresentFlags, void * pPresentParameters) {
  return dx_present1(self, SyncInterval, PresentFlags, pPresentParameters);
}

// --- End of Hooks
// ---------------------------------------------------------------------------------------------------- 

auto hooks::initialize() -> bool {
  if (MH_Initialize() != MH_OK)
    return false;

  std::uint8_t * base = (decltype(base))GetModuleHandle(NULL);
  __cr_hk(mc_entt_add_delta, base + 0x25E58E0);

  std::uint8_t * dxgi_base = (decltype(dxgi_base))GetModuleHandleA("dxgi.dll");
  if (!__cr_hk(dx_present, dxgi_base + 0x15e0) || !__cr_hk(dx_present1, dxgi_base + 0x68dc0)) {
    TerminateProcess(GetCurrentProcess(), 0);
  }

  return MH_EnableHook(MH_ALL_HOOKS);
}


auto hooks::uninitialize() -> bool {
  return MH_DisableHook(MH_ALL_HOOKS);
}


#undef __cr_hk
#undef __mk_hk
