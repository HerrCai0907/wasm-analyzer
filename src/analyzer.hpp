#pragma once

#include "module.hpp"
#include <utility>

namespace wa {

class Analyzer {
  Module m_module;

public:
  Analyzer(Module module) : m_module(std::move(module)) {}

  void analyze() {}
};

} // namespace wa
