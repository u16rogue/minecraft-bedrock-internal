#include "values.hpp"

#include <common/utils.hpp>
#include <common/pattern_scanner.hpp>

auto values::initialize() -> bool {
  auto [mc_base, mc_size] = mcbre::utils::module_info(NULL);
  mcbre::pattern_scan(mc_base, mc_size, mcbre::pattern<"48 89 5C 24 ? 57 48 83 EC 20 48 8B 41 18 49 8B F8 48">().frags, values::mc_string_container_set_method);
  mcbre::pattern_scan(mc_base, mc_size, mcbre::pattern<"40 53 48 83 EC ?? 48 8B 51 ?? 48 8B D9 48 83 FA ?? 72 2C">().frags, values::mc_string_container_constructor);
  mcbre::pattern_scan(mc_base, mc_size, mcbre::pattern<"40 55 57 41 56 48 83 EC ?? 48 8B 81">().frags, values::mc_append_chat_log);
  return true;
}
