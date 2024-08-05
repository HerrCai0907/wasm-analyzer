#pragma once

#include "analyzer.hpp"
#include "cfg.hpp"
#include <cstddef>
#include <memory>
#include <vector>

namespace wa {

class ExtendBasicBlockBuilder : public IAnalyzer {
  std::vector<ExtendCfg> m_extend_cfgs{};

public:
  explicit ExtendBasicBlockBuilder(std::shared_ptr<AnalyzerContext> context) : IAnalyzer(context) {}
  void analyze_impl(Module &module) override;
};

} // namespace wa
