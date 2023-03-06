#pragma once

namespace manager::plugins {
  auto load(const char * file) -> bool;
  auto unload(const char * file) -> bool;

  auto get_client_interface() -> void *;

  auto dispose() -> bool; // Unloads all plugins
}
