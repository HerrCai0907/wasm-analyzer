#pragma once

#include "module.hpp"
#include <vector>

namespace wa {

class Parser {
  std::vector<uint8_t> m_binary;

public:
  Parser(const char *path);

  Module parse();
};

} // namespace wa
