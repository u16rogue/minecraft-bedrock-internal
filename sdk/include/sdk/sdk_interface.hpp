#pragma once

#include <cstdint>
#include <cstddef>

namespace sdk {

class sdk_intf {
public:
  virtual ~sdk_intf() = 0;
  virtual auto query(std::uint64_t id, void * ptr, std::uint64_t size) -> bool = 0;
  virtual auto query(const char * id,  void * ptr, std::uint64_t size) -> bool = 0;

  template <typename T>
  auto query(std::uint64_t id, T * ptr) -> bool {
    return this->query(id, ptr, sizeof(T));
  }

  template <typename T>
  auto query(const char * id, T * ptr) -> bool {
    return this->query(id, ptr, sizeof(T));
  }
};

}
