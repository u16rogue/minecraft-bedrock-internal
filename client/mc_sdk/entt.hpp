#pragma once

#include <sdk/vec.hpp>

namespace mcsdk {

struct entt {
  void * vtable;
  char pad0[0x248];
  mcbre::sdk::vec3 * pcoords;
};

}
