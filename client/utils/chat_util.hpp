#pragma once

#include <string_view>

namespace game::utils::chat {
  auto add(const char * str) -> void;
  auto on_chat_tick_cb(void * self_) -> void;

  using reg_com_cb_t = bool(*)(const char *);
  auto register_command(std::string_view command, reg_com_cb_t cb) -> bool;
  auto on_send_message_cb(void * self_) -> bool;
}
