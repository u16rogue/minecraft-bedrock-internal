#pragma once

#include <sdk/client_intf.hpp>

namespace sdk {

struct ver_info {
  int major;
  int minor;
};

inline const ver_info verion = {
  .major = 1,
  .minor = 0
};

class client_intf : public sdk::sdk_intf {
public:
  virtual ~client_intf() = 0;
};

/*
 *
 */
using create_client_interface_t = bool(*)(ver_info * use_ver, client_intf * intf_out, ver_info * client_ver_opt);

} // sdk
