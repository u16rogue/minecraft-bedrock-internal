#pragma once

#define mcbre_glue_proxy(x, y) x##y
#define mcbre_glue(x, y) mcbre_glue_proxy(x, y)

#define mcbre_va_list(...) __VA_ARGS__

namespace mcbre::metapp::details {

template <typename T>
struct __defer {
  __defer(T && fn_) : fn(fn_) {}
  ~__defer() { fn(); }
  T fn;
};

template <typename T>
struct __imm {
  __imm(T && fn) {
    fn();
  }
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
      _data[i] = str[i];
    }
  }

  using _T = T[sz];
  _T _data;

  consteval auto data() const -> const _T& {
    return _data;
  }

  consteval auto length() const -> int {
    return sz;
  }
};

}

#define mcbre_defer \
  const mcbre::metapp::details::__defer mcbre_glue(__defer_, __COUNTER__) = [&]()

#define mcbre_imm \
  mcbre::metapp::details::__imm mcbre_glue(__imm, __COUNTER__) = []()

