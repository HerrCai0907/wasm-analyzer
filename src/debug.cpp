#include "debug.hpp"
#include "args.hpp"

namespace wa {

static const Arg<bool> debug_mode{"--debug", false};

bool Debug::is_debug_mode() { return debug_mode; }

} // namespace wa
