#include <Windows.h>
#include <common/logging.hpp>
#include <cstdio>
#include <MinHook.h>

auto __stdcall init_main(HMODULE hmod) -> DWORD {
  //mcbre_init_console();
  MH_Initialize();
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
    default:
      break;
  }
  return TRUE;
}
