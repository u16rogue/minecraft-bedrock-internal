#pragma once

namespace manager::plugins {
  auto load(const char * file) -> bool;
  auto unload(const char * file) -> bool;

  auto is_plugload_thread() -> bool;
  auto get_client_interface() -> void *;
  auto set_plugload_instance(void * inst) -> bool;

  auto initialize() -> bool;
  auto dispose() -> bool; // Unloads all plugins
}
