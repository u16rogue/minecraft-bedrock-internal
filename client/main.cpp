#include <Windows.h>
#include <cstdio>
#include <cstdint>

#include <MinHook.h>

#include <common/logging.hpp>

#include <game.hpp>
#include <hooks.hpp>
#include <plugin_manager.hpp>

#include <game_utils/chat_util.hpp>

static auto load_plugin(const char * msg) -> bool {
  return false;
}

static auto __stdcall init_main(HMODULE hmod) -> DWORD {
  game::initialize();
  hooks::initialize();
  manager::plugins::initialize();
  game::utils::chat::add("[" MCBRE_NAME "] mcbre client successfully loaded!");
  return 0;
}

auto __stdcall DllMain(HMODULE hmod, DWORD reason, LPVOID reserved) -> BOOL {
  switch (reason) {
    case DLL_PROCESS_ATTACH: {
      HANDLE proc_hnd = CreateThread(nullptr, NULL, LPTHREAD_START_ROUTINE(init_main), hmod, NULL, nullptr);
      if (proc_hnd)
        CloseHandle(proc_hnd);
      break;
    }

    case DLL_PROCESS_DETACH: {
      manager::plugins::dispose();
      hooks::uninitialize();
    }

    default:
      break;
  }
  return TRUE;
}
