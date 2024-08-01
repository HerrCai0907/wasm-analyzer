#pragma once

#include "analyzer.hpp"
#include "cfg.hpp"
#include "module.hpp"
#include <memory>
#include <vector>

namespace wa {

class CfgBuilder : public IAnalyzer {
  std::vector<Cfg> m_cfg{};

public:
  explicit CfgBuilder(std::shared_ptr<AnalyzerContext> const &context) : IAnalyzer(context) {}
  void analyze_impl(Module &module) override;

  std::vector<Cfg> const &get_cfg() const { return m_cfg; };
};

} // namespace wa
