#pragma once

namespace mc {

// 18/02/2023 - im not calling it vector.
template <typename T>
struct list_container {
  T * pdata;
  T * pend;

  auto begin() -> T * { return pdata; }
  auto end() -> T * { return pend; }

  auto operator[](int idx) -> T& {
    return pdata[idx];
  }
};

}
