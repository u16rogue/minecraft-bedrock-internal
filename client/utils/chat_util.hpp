#pragma once

namespace game::utils::chat {
  auto add(const char * str) -> void;
  auto on_chat_tick_cb(void * self_) -> void;
}
