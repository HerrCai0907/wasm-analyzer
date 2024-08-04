#pragma once

#include <array>

namespace wa {

struct ThirdPartyDeps {
  const char *m_name;
  const char *m_link;
};

extern std::array<ThirdPartyDeps, 1> third_party;

} // namespace wa
