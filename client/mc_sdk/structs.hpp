#pragma once

#include "list_container.hpp"
#include "string_container.hpp"

namespace mc {

struct chat_message_data {
  char pad0[40];
  __int64 unk_var;
  char pad1[56];
  mc::string_container msg_data;
  bool unk_check;
  char pad2[3];
  float maybe_fade_timer;
  char pad3[8];
};
static_assert(sizeof(chat_message_data) == 0x98, "mc::chat_message_data has an invalid size.");

struct unk_0 {
  char pad0[0x140];
  mc::list_container<chat_message_data> messages;
};

}
