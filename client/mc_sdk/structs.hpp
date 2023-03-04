#pragma once

#include "list_container.hpp"
#include "string_container.hpp"

namespace mc {

struct chat_message_data {
  void * unk0;
  mc::string_container message;
  mc::string_container context;
  mc::string_container sender_name;
  mc::string_container display_message;
  std::int32_t unkflag;
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

struct maybe_chat_instance {
  char pad0[0xA80];
  void * chat_manager; // 0xA80
  char pad1[0x8];
  void ** unk0;
  mc::string_container message; // 0xA98
  char pad2[0x32];
  bool unk1; // 0xAEA
  bool unk2; // 0xEAB
};

static_assert(
    offsetof(maybe_chat_instance, chat_manager) == 0xA80
 && offsetof(maybe_chat_instance, message)      == 0xA98
 && offsetof(maybe_chat_instance, unk1)         == 0xAEA,
    "offset is wrong");

}
