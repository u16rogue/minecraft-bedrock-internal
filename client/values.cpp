#include "values.hpp"

#include <common/utils.hpp>
#include <common/pattern_scanner.hpp>

auto values::initialize() -> bool {
  auto [mc_base, mc_size] = mcbre::utils::module_info(NULL);
  mcbre::pattern_scan(mc_base, mc_size, mcbre::pattern<"48 89 5C 24 ? 57 48 83 EC 20 48 8B 41 18 49 8B F8 48">().frags, values::mc_string_container_set_method);
  return true;
}
