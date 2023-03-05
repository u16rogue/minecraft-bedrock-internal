#include <plugin_manager.hpp>
#include <vector>
#include <Windows.h>

struct plug_entry {
  bool    occupied;
  HMODULE hmod;
};

static std::vector<plug_entry> plugins;

auto manager::plugins::load(const char * file) -> bool { 
  return false;
}

auto manager::plugins::unload(const char * file) -> bool {
  return false;
}
