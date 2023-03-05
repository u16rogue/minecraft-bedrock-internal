#pragma once

#include <mc_sdk/vec.hpp>

namespace mc {

struct entt {
  void * vtable;
  char pad0[0x248];
  mc::vec3 * pcoords;
};

}
