#include "chat_util.hpp"
#include <deque>
#include <string>

#include "../game.hpp"

#include "../mc_sdk/string_container.hpp"
#include "../mc_sdk/structs.hpp"

static std::deque<std::string> message_queue;

auto game::utils::chat::add(const char * str) -> void {
  message_queue.emplace_back(str);
}

auto game::utils::chat::on_chat_tick_cb(void * self_) -> void {
  mc::chat_manager * self = reinterpret_cast<decltype(self)>(self_);
  if (message_queue.empty() || !game::mc_append_chat_log)
    return;

  while (!message_queue.empty()) {
    mc::chat_message_data msg {};

    msg.display_message.construct();
    msg.display_message = message_queue.front().c_str();
    msg.maybe_fade_timer = 10.f;
    *reinterpret_cast<void **>(&msg.pad3) = 0;
    game::mc_append_chat_log(self, &msg, 4);
    message_queue.pop_front();
  }
}
