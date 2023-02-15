#include "hooks.hpp"

#include <vector>
#include <tuple>
// --
#include <Windows.h>
#include <psapi.h>
#include <d3d12.h>
// --
#include <MinHook.h>
// --
#include <common/pattern_scanner.hpp>
#include <sdk/vec.hpp>
#include "mc_sdk/entt.hpp"

#define __mk_mod(modn) \
  auto [base, sz] = module_info(modn)

#define __mk_hk(rt, name, ...)                \
  static rt(*name)(__VA_ARGS__) = nullptr;    \
  static auto __hk_##name(__VA_ARGS__) -> rt; \
  static auto __hk_##name(__VA_ARGS__) -> rt

#define __cr_hk(name, addr) \
  (MH_CreateHook(reinterpret_cast<void *>(addr), reinterpret_cast<void *>(&__hk_##name), reinterpret_cast<void **>(&name)) == MH_OK)

// NOTE
// [16/02/2023] - According to docu it should be able to capture refences to structure bindings (https://en.cppreference.com/w/cpp/language/structured_binding)
// but apparently not. This fails to compile (clang 15.x) when capturing the structured bindings.
#define __cr_hk_pattern(name, sig, ...)                                                                                                             \
  [](void * base, int sz) -> bool {                                                                                                                 \
    void * __##name##_sig = nullptr;                                                                                                                \
    return (mcbre::pattern_scan(base, sz, mcbre::pattern<sig>().frags, __##name##_sig __VA_OPT__(,) __VA_ARGS__) && __cr_hk(name, __##name##_sig)); \
  } (base, sz)


// ---------------------------------------------------------------------------------------------------- 
// --- Hooks

__mk_hk(void, mc_entt_add_delta, mc::entt * self, mcbre::sdk::vec3 * delta) {
  return mc_entt_add_delta(self, delta);
}

__mk_hk(HRESULT, dx_present, void * self, UINT SyncInterval, UINT Flags) {
  return dx_present(self, SyncInterval, Flags);
}

__mk_hk(HRESULT, dx_present1, void * self, UINT SyncInterval, UINT PresentFlags, void * pPresentParameters) {
  return dx_present1(self, SyncInterval, PresentFlags, pPresentParameters);
}

__mk_hk(LRESULT, wndproc, HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
  if (msg == WM_KEYDOWN && wp == 0x57)
    return TRUE;
  return wndproc(hwnd, msg, wp, lp);
}

// --- End of Hooks
// ---------------------------------------------------------------------------------------------------- 

static auto module_info(const char * name) -> std::pair<std::uint8_t *, std::size_t> {
  MODULEINFO mi = {};
  GetModuleInformation(GetCurrentProcess(), GetModuleHandleA(name), &mi, sizeof(mi));
  return std::make_pair(reinterpret_cast<std::uint8_t *>(mi.lpBaseOfDll), static_cast<std::size_t>(mi.SizeOfImage));
}

auto hooks::initialize() -> bool {
  if (MH_Initialize() != MH_OK)
    return false;

  if (__mk_mod(NULL);
    !__cr_hk_pattern(mc_entt_add_delta, "57 48 83 EC 20 48 8B 81 ? ? ? ? 48 8B DA 48 8B F9", -5)
  ) {
  }

  if (__mk_mod("dxgi.dll");
     !__cr_hk_pattern(dx_present,  "48 8B 05 ? ? ? ? 48 33 C4 48 89 45 60 45",       -26)
  || !__cr_hk_pattern(dx_present1, "48 8B 05 ? ? ? ? 48 33 C4 48 89 45 27 49 8B 41", -24)
  ) {
    TerminateProcess(GetCurrentProcess(), 0);
  }

  return MH_EnableHook(MH_ALL_HOOKS);
}


auto hooks::uninitialize() -> bool {
  return MH_DisableHook(MH_ALL_HOOKS);
}


#undef __cr_hk_pattern
#undef __cr_hk
#undef __mk_hk
