#include <module_manager.hpp>

#include <vector>

template <typename... modules>
struct static_module_dispatcher {
};

struct dmod_entry {
  bool occupied = false;
};


// static modules
using  smods = static_module_dispatcher<>; 

// dynamic modules
static std::vector<dmod_entry> dmods;
