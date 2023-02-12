#pragma once

#include <sdk/vec.hpp>

namespace mc {

struct entt {
  void * vtable;
  char pad0[0x248];
  mcbre::sdk::vec3 * pcoords;
};

}
