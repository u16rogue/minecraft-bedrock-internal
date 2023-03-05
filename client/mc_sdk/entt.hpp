#pragma once

#include "vec.hpp"

namespace mc {

struct entt {
  void * vtable;
  char pad0[0x248];
  mc::vec3 * pcoords;
};

}
