#include <Windows.h>
#include <cstdio>
#include <cstdint>

#include <MinHook.h>

#include <common/logging.hpp>

#include "game.hpp"
#include "hooks.hpp"

#include "utils/chat_util.hpp"

auto __stdcall init_main(HMODULE hmod) -> DWORD {
  game::initialize();
  hooks::initialize();
  game::utils::chat::register_command("greet", +[](const char *) {
    game::utils::chat::add("Hello everyone!");
    return true;
  });
  game::utils::chat::add("mcbre client successfully loaded!");
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
      hooks::uninitialize();
    }

    default:
      break;
  }
  return TRUE;
}
