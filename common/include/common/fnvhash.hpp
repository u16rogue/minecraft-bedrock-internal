#pragma once

#include <cstdint>
#include <string_view>

namespace mcbre::hash {

auto fnv32(const void * data, int sz) -> std::uint32_t;
auto fnv32(std::string_view str) -> std::uint32_t;

}
