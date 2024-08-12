#pragma once

#include <span>

namespace wa {

struct ThirdPartyDeps {
  const char *m_name;
  const char *m_link;
};

extern std::span<ThirdPartyDeps> third_party;

} // namespace wa
