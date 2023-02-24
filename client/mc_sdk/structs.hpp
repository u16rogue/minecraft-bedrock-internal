#pragma once

#include "list_container.hpp"
#include "string_container.hpp"

namespace mc {

struct chat_message_data {
  char pad0[40];
  __int64 unk_var;
  char pad1[56];
  mc::string_container str;
  bool unk_check; // 19/02/2023 - this can just be int and merged with pad2, i think its compiler pad aligned
  char pad2[3];
  float maybe_fade_timer;
  char pad3[8];
};
static_assert(sizeof(chat_message_data) == 0x98, "mc::chat_message_data has an invalid size.");

struct unk_0 {
  char pad0[0x140];
  mc::list_container<chat_message_data> messages;
};

struct keybind_info {
  mc::string_container key_tag; // name of the key
  mc::list_container<std::uint32_t> keys;
  bool bflag1;
  bool bflag2;
  char pad1[6];
};

struct keybind_manager {
  struct {
    void * pad0[9];
    std::int64_t(*get_last_key_pressed)(keybind_manager * self);
  } * vtable;
  mc::list_container<keybind_info> key_info;
};

}
