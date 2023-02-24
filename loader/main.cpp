#include <cstdio>
#include <filesystem>
#include <optional>
#include <string>
#include <string_view>

#include <AclAPI.h>
#include <TlHelp32.h>
#include <Windows.h>
#include <sddl.h>

#include <common/logging.hpp>
#include <common/metapp.hpp>

const char *          mc_im_name  = "Minecraft.Windows.exe";
std::filesystem::path client_name = "client.dll";

static auto get_bin_path() -> std::optional<std::filesystem::path> {
  char b[256]{};
  GetModuleFileNameA(GetModuleHandle(NULL), b, sizeof(b));
  auto s = std::string_view(b);
  auto i = s.find_last_of("/\\");
  if (i == std::string::npos)
    return std::nullopt;
  return std::make_optional(s.substr(0, i));
}

static auto set_security_details(std::filesystem::path file, const char * name) -> bool {
  EXPLICIT_ACCESS_A ea[] = {
    {
      .grfAccessPermissions = GENERIC_ALL,
      .grfAccessMode        = SET_ACCESS,
      .grfInheritance       = NO_INHERITANCE,
      .Trustee              = {
                               .TrusteeForm = TRUSTEE_IS_NAME,
                               .TrusteeType = TRUSTEE_IS_WELL_KNOWN_GROUP,
                               .ptstrName   = (LPCH)"SYSTEM",
                               },
    },
    {
      .grfAccessPermissions = GENERIC_ALL,
      .grfAccessMode        = SET_ACCESS,
      .grfInheritance       = NO_INHERITANCE,
      .Trustee              = {
                               .TrusteeForm = TRUSTEE_IS_NAME,
                               .TrusteeType = TRUSTEE_IS_WELL_KNOWN_GROUP,
                               .ptstrName   = (LPCH)"Users",
                               },
    },
    {
      .grfAccessPermissions = GENERIC_ALL,
      .grfAccessMode        = SET_ACCESS,
      .grfInheritance       = NO_INHERITANCE,
      .Trustee              = {
                               .TrusteeForm = TRUSTEE_IS_NAME,
                               .TrusteeType = TRUSTEE_IS_WELL_KNOWN_GROUP,
                               .ptstrName   = (LPCH)name,
                               },
    },
  };

  PSECURITY_DESCRIPTOR sd   = nullptr;
  PACL                 nacl = nullptr; // New ACL
  if (SetEntriesInAcl(mcbre::metapp::array_size(ea), ea, nullptr, &nacl) != ERROR_SUCCESS) {
    mcbre_log("Failure for SetEntriesInAcl");
    return false;
  }
  mcbre_defer {
    if (sd && nacl) LocalFree(nacl);
  };

  sd = LocalAlloc(LPTR, SECURITY_DESCRIPTOR_MIN_LENGTH);
  if (!sd) {
    mcbre_log("Failure for LocalAlloc");
    return false;
  }
  mcbre_defer {
    if (sd) LocalFree(sd);
  };

  if (!InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION) || !SetSecurityDescriptorDacl(&sd, TRUE, nacl, FALSE)) {
    mcbre_log("Failure for InitializeSecurityDescriptor or SetSecurityDescriptorDacl");
    return false;
  }
  nacl = nullptr;

  if (!SetFileSecurity(file.string().c_str(), DACL_SECURITY_INFORMATION, &sd)) {
    mcbre_log("Failure for SetFileSecurity");
    return false;
  }
  sd = nullptr;

  return true;
}

auto main(int argc, char ** argv) -> int {

#if defined(MCBRE_DEBUG_BUILD) && MCBRE_DEBUG_BUILD
  mcbre_log("Debug build.");
#else
  mcbre_log("Non debug build.");
#endif

  bool unload_only = false;

  if (argc > 1) {
    for (int i = 1; i < argc; ++i) {
      if (strcmp(argv[i], "-u") == 0 || strcmp(argv[i], "--unload") == 0) {
        mcbre_log("Only unloading. Exiting after unload request.");
        unload_only = true;
      }
    }
  }

  auto ap = get_bin_path(); // absolute path
  if (!ap)
    return 1;
  if (!std::filesystem::exists(client_name)) {
    client_name = *ap / client_name;
    if (!std::filesystem::exists(client_name))
      return 1;
  }
  mcbre_log("Client: {}", client_name.string());

  std::string full_path = std::filesystem::absolute(client_name).string();
  mcbre_log("Absolute path: {}", full_path);

  if (!unload_only && !set_security_details(full_path.c_str(), "ALL APPLICATION PACKAGES"))
    return 8;

  HANDLE procsnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
  mcbre_log("Process snapshot: {}", procsnap);
  if (procsnap == INVALID_HANDLE_VALUE)
    return 2;
  mcbre_defer {
    CloseHandle(procsnap);
  };

  DWORD          pid = NULL;
  PROCESSENTRY32 pe  = { .dwSize = sizeof(pe) };
  if (Process32First(procsnap, &pe)) {
    do {
      if (strcmp(pe.szExeFile, mc_im_name) == 0) {
        pid = pe.th32ProcessID;
        break;
      }
    } while (Process32Next(procsnap, &pe));
  }
  mcbre_log("PID: {}", pid);
  if (pid == NULL)
    return unload_only ? 0 : 3;

  HANDLE modsnap = NULL;
  while ((modsnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, pid)) == INVALID_HANDLE_VALUE && GetLastError() == ERROR_BAD_LENGTH) mcbre_log("Bad length on module snapshot. Retrying...");
  mcbre_log("Module snapshot: {}", modsnap);
  if (modsnap == INVALID_HANDLE_VALUE)
    return 9;
  mcbre_defer {
    CloseHandle(modsnap);
  };

  HANDLE hproc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
  mcbre_log("Process handle: {}", hproc);
  if (hproc == INVALID_HANDLE_VALUE)
    return 4;
  mcbre_defer {
    CloseHandle(hproc);
  };

  MODULEENTRY32 me = { .dwSize = sizeof(me) };
  if (Module32First(modsnap, &me)) {
    do {
      if (strcmp(me.szExePath, full_path.c_str()) == 0) {
        mcbre_log("Unloading client...");
        HANDLE hthread = CreateRemoteThread(hproc, nullptr, NULL, LPTHREAD_START_ROUTINE(&FreeLibrary), me.modBaseAddr, NULL, nullptr);
        mcbre_log("Unload thread: {}", hthread);
        if (hthread == INVALID_HANDLE_VALUE)
          return 10;
        mcbre_defer {
          CloseHandle(hthread);
        };
        WaitForSingleObject(hthread, INFINITE);
      }
    } while (Module32Next(modsnap, &me));
  }

  if (unload_only)
    return 0;

  std::size_t alloc_sz = full_path.length() + 1;
  mcbre_log("Allocation size: {}", alloc_sz);

  LPVOID palloc = VirtualAllocEx(hproc, NULL, alloc_sz, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
  mcbre_log("Remote allocated at {}", palloc);
  if (!palloc)
    return 5;
  mcbre_defer {
    VirtualFreeEx(hproc, palloc, NULL, MEM_RELEASE);
  };

  if (!WriteProcessMemory(hproc, palloc, full_path.c_str(), alloc_sz, nullptr))
    return 6;

  HANDLE hthread = CreateRemoteThread(hproc, NULL, NULL, LPTHREAD_START_ROUTINE(&LoadLibraryA), palloc, NULL, NULL);
  mcbre_log("Load Thread handle: {}", hthread);
  if (hthread == INVALID_HANDLE_VALUE)
    return 7;

  WaitForSingleObject(hthread, INFINITE);

  mcbre_log("Successfuly loaded (i think).");
  return 0;
}
