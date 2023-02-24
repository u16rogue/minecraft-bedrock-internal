#include <common/utils.hpp>

#include <Windows.h>
#include <psapi.h>

auto mcbre::utils::module_info(const char * name) -> std::pair<std::uint8_t *, std::size_t> {
  MODULEINFO mi = {};
  GetModuleInformation(GetCurrentProcess(), GetModuleHandleA(name), &mi, sizeof(mi));
  return std::make_pair(reinterpret_cast<std::uint8_t *>(mi.lpBaseOfDll), static_cast<std::size_t>(mi.SizeOfImage));
}

