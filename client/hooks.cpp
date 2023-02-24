#include "hooks.hpp"

#include <vector>
#include <tuple>
#include <unordered_map>
#include <string>
// --
#include <Windows.h>
#include <psapi.h>
#include <d3d12.h>
// --
#include <MinHook.h>
// --
#include <common/pattern_scanner.hpp>
#include <common/fnvhash.hpp>
#include <sdk/vec.hpp>
#include "mc_sdk/entt.hpp"
#include "mc_sdk/structs.hpp"

#include <common/logging.hpp>

#define __mk_mod(modn) \
  auto [base, sz] = module_info(modn)

#define __mk_hk(rt, name, ...)                \
  static rt(*name)(__VA_ARGS__) = nullptr;    \
  static auto __hk_##name(__VA_ARGS__) -> rt

#define __cr_hk(name, addr) \
  (MH_CreateHook(reinterpret_cast<void *>(addr), reinterpret_cast<void *>(&__hk_##name), reinterpret_cast<void **>(&name)) == MH_OK)

#define offsets(...) __VA_OPT__(,) __VA_ARGS__

#define mcbre_mk_hk_by_sig(modname, sig, offsets_, rt, name, ...)                             \
  static rt(*name)(__VA_ARGS__) = nullptr;                                                    \
  static auto __hk_##name(__VA_ARGS__) -> rt;                                                 \
  static mcbre_imm {                                                                          \
    registered_hooks.emplace_back(hook_entry {+[](void * base, int sz){                       \
        void * __##name##_sig = nullptr;                                                      \
        mcbre::pattern_scan(base, sz, mcbre::pattern<sig>().frags, __##name##_sig offsets_);  \
        return __##name##_sig;                                                                \
    }, modname, &name, reinterpret_cast<void *>(&__hk_##name), mcbre::hash::cvfnv32(#name)}); \
  };                                                                                          \
  static auto __hk_##name(__VA_ARGS__) -> rt

#define mcbre_mk_hk_by_export(modname, rt, name, ...)                                            \
  static rt(*name##_)(__VA_ARGS__) = nullptr;                                                    \
  static auto __hk_##name(__VA_ARGS__) -> rt;                                                    \
  static mcbre_imm {                                                                             \
    registered_hooks.emplace_back(hook_entry {+[](void * base, int sz){                          \
        (void)sz;                                                                                \
        return reinterpret_cast<void *>(GetProcAddress(reinterpret_cast<HMODULE>(base), #name)); \
    }, modname, &name##_, reinterpret_cast<void *>(&__hk_##name), mcbre::hash::cvfnv32(#name)}); \
  };                                                                                             \
  static auto __hk_##name(__VA_ARGS__) -> rt


static auto module_info(const char * name) -> std::pair<std::uint8_t *, std::size_t> {
  MODULEINFO mi = {};
  GetModuleInformation(GetCurrentProcess(), GetModuleHandleA(name), &mi, sizeof(mi));
  return std::make_pair(reinterpret_cast<std::uint8_t *>(mi.lpBaseOfDll), static_cast<std::size_t>(mi.SizeOfImage));
}

struct hook_entry {
  void*(*lookup)(void * base, int sz);
  const char * modname;
  void * pout;
  void * hkfn;

  const std::uint32_t id;

  bool hooked = false;
};

static std::vector<hook_entry> registered_hooks;

// ---------------------------------------------------------------------------------------------------- 
// --- Hooks

mcbre_mk_hk_by_sig("Minecraft.Windows.exe", "48 69 C9 ? ? ? ? 48 99 48 F7 FF 48 03 C1 48 8B 93", offsets(-58),
void *, mc_maybe_chat_tick, mc::unk_0 * self) {
  // Message update check
  static void * last_end = nullptr;
  if (auto * cend = self->messages.end(); last_end != cend) {
    last_end = cend;
    mc::chat_message_data * recent_msg = (cend - 1);
    // Dispatch chat update 
    mcbre_log_dbg("Sent a message {}", recent_msg->str.c_str());
  }
  
  return mc_maybe_chat_tick(self);
};

mcbre_mk_hk_by_sig("Minecraft.Windows.exe", "57 48 83 EC 20 48 8B 81 ? ? ? ? 48 8B DA 48 8B F9", offsets(-5),
void, mc_entt_add_delta, mc::entt * self, mcbre::sdk::vec3 * delta) {
  delta->y = 0.01;
  return mc_entt_add_delta(self, delta);
};

mcbre_mk_hk_by_sig("dxgi.dll", "48 8B 05 ? ? ? ? 48 33 C4 48 89 45 60 45", offsets(-26),
HRESULT, dx_present, void * self, UINT SyncInterval, UINT Flags) {
  return dx_present(self, SyncInterval, Flags);
}

mcbre_mk_hk_by_sig("dxgi.dll", "48 8B 05 ? ? ? ? 48 33 C4 48 89 45 27 49 8B 41", offsets(-24),
HRESULT, dx_present1, void * self, UINT SyncInterval, UINT PresentFlags, void * pPresentParameters) {
  return dx_present1(self, SyncInterval, PresentFlags, pPresentParameters);
}

#if 0
mcbre_mk_hk_by_export("User32.dll",
BOOL, PeekMessageW, LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax, UINT wRemoveMsg) {
  return PeekMessageW_(lpMsg, hWnd, wMsgFilterMin, wMsgFilterMax, wRemoveMsg);
}

mcbre_mk_hk_by_export("ntdll.dll",
LRESULT, NtdllDefWindowProc_W, HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
  return NtdllDefWindowProc_W_(hwnd, msg, wparam, lparam);
}
#endif

// --- End of Hooks
// ---------------------------------------------------------------------------------------------------- 

auto hooks::initialize() -> bool {
  if (MH_Initialize() != MH_OK)
    return false;

  std::unordered_map<std::string, std::pair<std::uint8_t *, int>> module_cache = {};
  for (auto & hk : registered_hooks) {
    if (module_cache.find(hk.modname) == module_cache.end())
      module_cache[hk.modname] = module_info(hk.modname);
    const auto & [base, sz] = module_cache[hk.modname];
    if (!base)
      continue;
    void * target = hk.lookup(base, sz);
    if (!target)
      continue;
    hk.hooked = MH_CreateHook(target, hk.hkfn, reinterpret_cast<void **>(hk.pout)) == MH_OK;
  }
  return MH_EnableHook(MH_ALL_HOOKS) == MH_OK;
}

auto hooks::uninitialize() -> bool {
  return MH_DisableHook(MH_ALL_HOOKS) == MH_OK;
}

#undef mcbre_mk_hk_by_export
#undef mcbre_mk_hk_by_sig
#undef offsets
#undef __cr_hk_pattern
#undef __cr_hk
#undef __mk_hk
