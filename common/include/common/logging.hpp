#pragma once

// #include <source_location>
#include <cstdio>
#include <format>
#include <string_view>

namespace mcbre::common {

template <typename T, int sz>
struct string_cont {
  consteval string_cont(const T (&s)[sz]) {
    for (int i = 0; i < sz; ++i)
      data[i] = s[i];
  }

  using native_t = const T *;
  operator native_t() {
    return data;
  };

  T data[sz];
};

template <typename... vargs_t>
auto with_loc_fmt(const char * file, const char * fn, const int line, const std::string_view & fmt, vargs_t... args) -> void {
  constexpr int offset_index = []() consteval { return std::string(__FILE__).find(MCBRE_PROJ_DIR_NAME) + sizeof(MCBRE_PROJ_DIR_NAME) - 1; }
  ();
  printf("\n[%s:%d#%s] %s", file + offset_index, line, fn, std::vformat(fmt, std::make_format_args(args...)).c_str());
}

} // namespace mcbre::common

#if defined(MCBRE_LOGGING) && MCBRE_LOGGING

  #define mcbre_log(...) \
    mcbre::common::with_loc_fmt(__FILE__, __FUNCTION__, __LINE__, __VA_ARGS__)

  #define mcbre_init_console()               \
    {                                        \
      AllocConsole();                        \
      FILE * f = nullptr;                    \
      freopen_s(&f, "CONOUT$", "w", stdout); \
      freopen_s(&f, "CONIN$", "r", stdin);   \
    }

#else
  #define mcbre_log(...)
  #define mcbre_init_console()
#endif
