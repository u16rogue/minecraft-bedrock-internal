#include <plugin_manager.hpp>
#include <vector>
#include <Windows.h>
#include <memory>

#include <sdk/client_interface.hpp>
#include <sdk/plugin_interface.hpp>

struct plug_entry {
  bool    occupied;
  HMODULE hmod;
  sdk::plugin_intf * instance;
};

static std::unique_ptr<sdk::client_intf> client;
static std::vector<plug_entry> plugins;

auto manager::plugins::load(const char * file) -> bool { 
  return false;
}

auto manager::plugins::unload(const char * file) -> bool {
  return false;
}

auto manager::plugins::dispose() -> bool {
  return false;
}

// -------------------------------------------------------------------------------------------------------------------------------
// -- Client interface implementation

#include <game.hpp>
#include <game_utils/chat_util.hpp>
#include <mc_sdk/string_container.hpp>

class impl_client_intf : public sdk::client_intf {
public:
  ~impl_client_intf() override {}

  virtual auto query(const char * id,  void * ptr, std::uint64_t size) -> bool override {
    return false;
  }

  virtual auto register_plugin(sdk::plugin_intf * instance) -> bool override {
    return false;
  }

  virtual auto unregister_plugin(sdk::plugin_intf * instance) -> bool override {
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

auto get_client_interface() -> void * {
  if (!client) {
    client = std::make_unique<impl_client_intf>();
  }
  return client.get();
}

