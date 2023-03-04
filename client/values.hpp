#pragma once

namespace values {

inline void * mc_string_container_set_method = nullptr;
inline void * (*mc_append_chat_log)(void *, void *, int) = nullptr;

auto initialize() -> bool;

}
