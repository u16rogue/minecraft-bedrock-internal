#include <plugin_manager.hpp>
#include <vector>
#include <Windows.h>

#include <sdk/plugin_interface.hpp>

struct plug_entry {
  bool    occupied;
  HMODULE hmod;
  sdk::plugin_intf * instance;
};

static std::vector<plug_entry> plugins;

auto manager::plugins::load(const char * file) -> bool { 
  return false;
}

auto manager::plugins::unload(const char * file) -> bool {
  return false;
}
