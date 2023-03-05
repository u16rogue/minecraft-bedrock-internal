#include "chat_util.hpp"
#include <deque>
#include <string>
#include <vector>

#include <game.hpp>
#include <mc_sdk/string_container.hpp>
#include <mc_sdk/structs.hpp>


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

static std::vector<std::pair<std::string, game::utils::chat::reg_com_cb_t>> rc_cbs;
auto game::utils::chat::register_command(std::string_view command, game::utils::chat::reg_com_cb_t cb) -> bool {
  for (const auto & [cm, cb] : rc_cbs)
    if (cm == command)
      return false;

  rc_cbs.emplace_back(command, cb);
  return true;
}

auto game::utils::chat::on_send_message_cb(void * self_) -> bool {
  mc::maybe_chat_ui_manager * self = reinterpret_cast<decltype(self)>(self_);
  // Should be 3 or more so it can be '.x'
  if (self->message.length <= 2 || self->message.c_str()[0] != '.')
    return false;

  std::string_view command = self->message.c_str() + 1;
  for (const auto & [cm, cb] : rc_cbs) {
    if (command.length() >= cm.size() && command.starts_with(cm)) {
      bool r = cb(command.data());
      if (r)
        self->message = "";
      return r;
    }
  }

  game::utils::chat::add("[" MCBRE_NAME "] Invalid command");
  return true;
}
