#include <common/utils.hpp>
#include <common/logging.hpp>
#include <common/metapp.hpp>
#include <Windows.h>
#include <psapi.h>
#include <AclAPI.h>
#include <sddl.h>

auto mcbre::utils::module_info(const char * name) -> std::pair<std::uint8_t *, std::size_t> {
  MODULEINFO mi = {};
  GetModuleInformation(GetCurrentProcess(), GetModuleHandleA(name), &mi, sizeof(mi));
  return std::make_pair(reinterpret_cast<std::uint8_t *>(mi.lpBaseOfDll), static_cast<std::size_t>(mi.SizeOfImage));
}

auto mcbre::utils::set_security_details(std::filesystem::path file, const char * name) -> bool {
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
