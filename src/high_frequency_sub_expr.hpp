#pragma once

#include "analyzer.hpp"
#include <memory>

namespace wa {

class HighFrequencySubExpr : public IAnalyzer {
public:
  explicit HighFrequencySubExpr(std::shared_ptr<AnalyzerContext> context) : IAnalyzer(context) {}
  void analyze_impl(Module &module) override;
};

} // namespace wa
