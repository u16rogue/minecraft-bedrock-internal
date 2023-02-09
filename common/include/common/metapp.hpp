#pragma once

#define mcbre_glue_proxy(x, y) x##y
#define mcbre_glue(x, y) mcbre_glue_proxy(x, y)

namespace mcbre::metapp::details {

template <typename T>
struct __defer {
  __defer(T && fn_) : fn(fn_) {}
  ~__defer() { fn(); }
  T fn;
};

} // namespace mcbre::metapp::details

namespace mcbre::metapp {

template <typename T, int sz>
auto array_size(const T (&v)[sz]) -> int {
  return sz;
}

template <typename T, int sz>
struct comptime_str {
  consteval comptime_str(const T (&str)[sz]) {
    for (int i = 0; i < sz; ++i) {
      data[i] = str[i];
    }
  }

  T data[sz];

  consteval auto length() -> int {
    return sz;
  }
};

}

#define mcbre_defer \
  const mcbre::metapp::details::__defer mcbre_glue(__defer_, __COUNTER__) = [&]()

