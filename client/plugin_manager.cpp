#include <plugin_manager.hpp>
#include <vector>
#include <Windows.h>
#include <memory>
#include <thread>
#include <string>
#include <deque>

#include <common/metapp.hpp>
#include <common/utils.hpp>

#include <sdk/client_interface.hpp>
#include <sdk/plugin_interface.hpp>

#include <game_utils/chat_util.hpp>

struct plug_entry {
  bool    occupied;
  HMODULE hmod;
  sdk::plugin_intf * instance;
};

static sdk::client_intf      * client = nullptr;

static std::thread::id loader_thread_id;
static bool  flag_loader_thread_running = false;
static bool  flag_keep_alive            = false;
static plug_entry * active_entry        = nullptr;

static std::thread             loader_thread;
static std::deque<std::string> load_queue;
static std::vector<plug_entry> plugins;

static auto loader_fn() -> void {
  loader_thread_id = std::this_thread::get_id();
  flag_loader_thread_running = true;

  while (flag_keep_alive) {
    std::this_thread::sleep_for(std::chrono::seconds(1));
    if (load_queue.empty())
      continue;

    while (!load_queue.empty()) {
      if (!flag_keep_alive)
        break;
      std::string splug = load_queue.front();
      load_queue.pop_front();
      game::utils::chat::add(("[" MCBRE_NAME "] Loading plugin: " + splug).c_str());

      // game::utils::chat::add("[" MCBRE_NAME "] Updating plugin security details...");
      // if (!mcbre::utils::set_security_details(splug, "ALL APPLICATION PACKAGES")) {
      //   game::utils::chat::add("[" MCBRE_NAME "] Couldnt update plugin security details.");
      // }

      plug_entry      pe {};
      active_entry = &pe;
      mcbre_defer { active_entry = nullptr; };

      HMODULE hm = LoadLibraryA(splug.c_str());
      if (!hm) {
        game::utils::chat::add("[" MCBRE_NAME "] Failed to load plugin.");
        continue;
      }

      // if (!pe.hmod || !pe.instance) {
      //   TerminateProcess(GetCurrentProcess(), 1);
      // }

      bool no_unoccupied = true;
      for (auto & entry : plugins) {
        if (entry.occupied)
          continue;

        entry          = pe;
        entry.occupied = true;
        no_unoccupied  = false;
        break;
      }

      if (no_unoccupied) {
        pe.occupied = true;
        plugins.emplace_back(pe);
      }

      game::utils::chat::add("[" MCBRE_NAME "] Plugin loaded!");
    }
  }

  flag_loader_thread_running = false;
}

auto manager::plugins::load(const char * file) -> bool { 
  load_queue.emplace_back(file);
  return true;
}

auto manager::plugins::unload(const char * file) -> bool {
  return false;
}

auto manager::plugins::is_plugload_thread() -> bool {
  return std::this_thread::get_id() == loader_thread_id;
}

auto manager::plugins::set_plugload_instance(void * inst) -> bool {
  if (!inst || !active_entry) {
    return false;
  }

  active_entry->instance = reinterpret_cast<sdk::plugin_intf *>(inst);
  return true;
}

auto manager::plugins::initialize() -> bool {
  flag_keep_alive = true;
  loader_thread = std::thread(loader_fn);
  while (!flag_loader_thread_running) {
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
  }
  return true;
}

auto manager::plugins::dispose() -> bool {
  if (!flag_loader_thread_running)
    return true;
  flag_keep_alive = false;
  loader_thread.join();
  while (flag_loader_thread_running) {
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
  }

  // Unload all plugins
 
  return true;
}

// -------------------------------------------------------------------------------------------------------------------------------
// -- Client interface implementation

#include <game.hpp>
#include <mc_sdk/string_container.hpp>

class impl_client_intf : public sdk::client_intf {
public:
  virtual auto query(const char * id,  void * ptr, std::uint64_t size) -> bool override {
    return false;
  }

  virtual auto register_module(sdk::plugin_intf * parent, sdk::module_intf * instance) -> bool override {
    return false;
  }

  virtual auto unregister_module(sdk::module_intf * instance) -> bool override {
    return false;
  }

  virtual auto enumerate_plugins(sdk::plugin_intf * out, std::size_t * count) -> bool override {
    return false;
  }

  virtual auto enumerate_modules(sdk::module_intf * out, std::size_t * count) -> bool override {
    return false;
  }

  virtual auto add_event_listener(const char * ename, void * fnp) -> bool override {
    return false;
  }

  virtual auto remove_event_listener(void * fnp) -> bool override {
    return false;
  }

  virtual auto queue_log_chat(const char * text) -> bool override {
    if (!game::mc_append_chat_log)
      return false;

    game::utils::chat::add(text);
    return true;
  }

  virtual auto get_mcstr(sdk::managed_string * ms) -> const char * override {
    return reinterpret_cast<mc::string_container *>(ms)->c_str();
  }

  virtual auto set_mcstr(sdk::managed_string * ms, const char * str) -> sdk::managed_string * override {
    mc::string_container * sc = reinterpret_cast<decltype(sc)>(ms);
    *sc = str;
    return ms;
  }
};

auto manager::plugins::get_client_interface() -> void * {
  if (!client) {
    client = new impl_client_intf();
  }
  return client;
}

