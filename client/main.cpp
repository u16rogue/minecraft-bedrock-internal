#include <Windows.h>
#include <cstdio>
#include <cstdint>

#include <MinHook.h>

#include <common/logging.hpp>
#include <sdk/vec.hpp>

#include "hooks.hpp"

auto __stdcall init_main(HMODULE hmod) -> DWORD {
  hooks::initialize();
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
