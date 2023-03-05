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
#include <common/utils.hpp>
#include "mc_sdk/vec.hpp"
#include "mc_sdk/entt.hpp"
#include "mc_sdk/structs.hpp"
// --
#include "game.hpp"
// --
#include "utils/chat_util.hpp"

#include <common/logging.hpp>

#define __mk_mod(modn) \
  auto [base, sz] = module_info(modn)

#define __mk_hk(rt, name, ...)                \
  static rt(*name)(__VA_ARGS__) = nullptr;    \
  static auto __hk_##name(__VA_ARGS__) -> rt

#define __cr_hk(name, addr) \
  (MH_CreateHook(reinterpret_cast<void *>(addr), reinterpret_cast<void *>(&__hk_##name), reinterpret_cast<void **>(&name)) == MH_OK)

#if defined(MCBRE_STORE_HOOK_NAME)  
  #define __hk_name_opt(name) , #name
#else
  #define __hk_name_opt(name)
#endif

#define offsets(...) __VA_OPT__(,) __VA_ARGS__

#define mcbre_mk_hk_by_sig(modname, sig, offsets_, rt, name, ...)                                                        \
  static rt(*name)(__VA_ARGS__) = nullptr;                                                                               \
  static auto __hk_##name(__VA_ARGS__) -> rt;                                                                            \
  static mcbre_imm {                                                                                                     \
    registered_hooks.emplace_back(hooks::hook_entry {+[](void * base, int sz){                                           \
        void * __##name##_sig = nullptr;                                                                                 \
        mcbre::pattern_scan(base, sz, mcbre::pattern<sig>().frags, __##name##_sig offsets_);                             \
        return __##name##_sig;                                                                                           \
    }, modname, &name, reinterpret_cast<void *>(&__hk_##name), mcbre::hash::cvfnv32(#name), false __hk_name_opt(name)}); \
  };                                                                                                                     \
  static auto __hk_##name(__VA_ARGS__) -> rt

#define mcbre_mk_hk_by_export(modname, rt, name, ...)                                                                       \
  static rt(*name##_)(__VA_ARGS__) = nullptr;                                                                               \
  static auto __hk_##name(__VA_ARGS__) -> rt;                                                                               \
  static mcbre_imm {                                                                                                        \
    registered_hooks.emplace_back(hooks::hook_entry {+[](void * base, int sz){                                              \
        (void)sz;                                                                                                           \
        return reinterpret_cast<void *>(GetProcAddress(reinterpret_cast<HMODULE>(base), #name));                            \
    }, modname, &name##_, reinterpret_cast<void *>(&__hk_##name), mcbre::hash::cvfnv32(#name), false __hk_name_opt(name)}); \
  };                                                                                                                        \
  static auto __hk_##name(__VA_ARGS__) -> rt

#define mcbre_mk_hk_by_ptr(modname, ptr, rt, name, ...)                                                                              \
  static rt(*name)(__VA_ARGS__) = nullptr;                                                                               \
  static auto __hk_##name(__VA_ARGS__) -> rt;                                                                               \
  static mcbre_imm {                                                                                                        \
    registered_hooks.emplace_back(hooks::hook_entry {+[](void * base, int sz){                                              \
        (void)sz;                                                                                                           \
        return reinterpret_cast<void *>(ptr);                                                                               \
    }, modname, &name, reinterpret_cast<void *>(&__hk_##name), mcbre::hash::cvfnv32(#name), false __hk_name_opt(name)}); \
  };                                                                                                                        \
  static auto __hk_##name(__VA_ARGS__) -> rt

#if defined(MCBRE_LOGGING) && MCBRE_LOGGING == 1
  #if defined(MCBRE_STORE_HOOK_NAME) && MCBRE_STORE_HOOK_NAME == 1
    #define __hk_get_name(_) _.idstr
  #else
    #define __hk_get_name(_) _.id
  #endif
#endif

static std::vector<hooks::hook_entry> registered_hooks;
static std::pair<std::uint8_t *, std::size_t> mc_module;

// ---------------------------------------------------------------------------------------------------- 
// --- Hooks

mcbre_mk_hk_by_sig("Minecraft.Windows.exe", "48 89 5C 24 ?? 48 89 74 24 ?? 48 89 7C 24 ?? 55 41 56 41 57 48 8B EC 48 83 EC ?? 48 8B F9", offsets(),
bool, mc_send_message_callback, mc::maybe_chat_ui_manager * self) {
  if (game::utils::chat::on_send_message_cb(self))
    return false;
  return mc_send_message_callback(self);
}

mcbre_mk_hk_by_ptr("Minecraft.Windows.exe", game::mc_append_chat_log,
void *, mc_chat_log, mc::chat_manager * self, mc::chat_message_data * entry, int unkarg0) {
  // If chat log append is sent outside of minecraft (most likely us) just call original
  void * rta = __builtin_return_address(0);
  if (rta < mc_module.first && rta > mc_module.first + mc_module.second)
    return mc_chat_log(self, entry, unkarg0);

  return mc_chat_log(self, entry, unkarg0);
}

mcbre_mk_hk_by_sig("Minecraft.Windows.exe", "48 89 74 24 ? 57 48 83 EC 20 48 8B 01 48 8B EA 48 8B F9 48", offsets(-5),
bool, mc_unk_fn0, void * self, void * unk) {
  #if 1 // fast place
  // 26/02/2023 - NOTE: Simply checks for mov edx, 0x11! that's what the 0x11 is for.
  // Checks if its a place request if so return a false to make the game think our place cooldown finished.
  if (reinterpret_cast<std::uint8_t *>(__builtin_return_address(0))[8] == 0x11)
    return false;
  #endif // fast place
    
  return mc_unk_fn0(self, unk);
}

mcbre_mk_hk_by_sig("Minecraft.Windows.exe", "48 69 C9 ? ? ? ? 48 99 48 F7 FF 48 03 C1 48 8B 93", offsets(-58),
void *, mc_maybe_chat_tick, mc::chat_manager * self) {
  game::utils::chat::on_chat_tick_cb(self);
  return mc_maybe_chat_tick(self);
}

mcbre_mk_hk_by_sig("Minecraft.Windows.exe", "57 48 83 EC 20 48 8B 81 ? ? ? ? 48 8B DA 48 8B F9", offsets(-5),
void, mc_entt_add_delta, mc::entt * self, mc::vec3 * delta) {
  return mc_entt_add_delta(self, delta);
}

mcbre_mk_hk_by_sig("dxgi.dll", "48 8B 05 ? ? ? ? 48 33 C4 48 89 45 60 45", offsets(-26),
HRESULT, dx_present, void * self, UINT SyncInterval, UINT Flags) {
  return dx_present(self, SyncInterval, Flags);
}

#if 0
mcbre_mk_hk_by_sig("dxgi.dll", "48 8B 05 ? ? ? ? 48 33 C4 48 89 45 27 49 8B 41", offsets(-24),
HRESULT, dx_present1, void * self, UINT SyncInterval, UINT PresentFlags, void * pPresentParameters) {
  return dx_present1(self, SyncInterval, PresentFlags, pPresentParameters);
}

mcbre_mk_hk_by_export("User32.dll",
BOOL, PeekMessageW, LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax, UINT wRemoveMsg) {
  return PeekMessageW_(lpMsg, hWnd, wMsgFilterMin, wMsgFilterMax, wRemoveMsg);
}
#endif

// --- End of Hooks
// ---------------------------------------------------------------------------------------------------- 

auto hooks::hook_info_get(std::uint32_t id) -> std::optional<const hooks::hook_entry> {
  for (const auto & entry : registered_hooks)
    if (entry.id == id)
      return std::make_optional(entry);
  return std::nullopt;
}

auto hooks::initialize() -> bool {
  if (MH_Initialize() != MH_OK)
    return false;

  mc_module = mcbre::utils::module_info("Minecraft.Windows.exe");
  if (!mc_module.first)
    return false;

  std::unordered_map<std::string, std::pair<std::uint8_t *, int>> module_cache = {};
  for (auto & hk : registered_hooks) {
    if (module_cache.find(hk.modname) == module_cache.end())
      module_cache[hk.modname] = mcbre::utils::module_info(hk.modname);
    const auto & [base, sz] = module_cache[hk.modname];
    if (!base)
      continue;
    void * target = hk.lookup(base, sz);
    hk.cached_target = target;
    if (!target)
      continue;
    hk.hooked = MH_CreateHook(target, hk.hkfn, reinterpret_cast<void **>(hk.pout)) == MH_OK && MH_EnableHook(target) == MH_OK;
    #if defined(MCBRE_LOGGING) && MCBRE_LOGGING == 1
    mcbre_log_dbg("Hook {} for {} @ {}", hk.hooked ? "successful" : "failed", __hk_get_name(hk), hk.cached_target);
    #endif
  }
  return true; // 24/02/2023 : MH_EnableHook(MH_ALL_HOOKS) == MH_OK;
}

auto hooks::uninitialize() -> bool {
  if (MH_DisableHook(MH_ALL_HOOKS) != MH_OK) {
    return false;
  }

  for (auto & hk : registered_hooks) {
    if (hk.cached_target && hk.hooked) {
      MH_RemoveHook(hk.cached_target);
    }
  }

  return MH_Uninitialize() == MH_OK;
}

#ifdef __hk_get_name
  #undef __hk_get_name
#endif
#undef mcbre_mk_hk_by_ptr
#undef mcbre_mk_hk_by_export
#undef mcbre_mk_hk_by_sig
#undef offsets
#undef __hk_name_opt
#undef __cr_hk
#undef __mk_hk
