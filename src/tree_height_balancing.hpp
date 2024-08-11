#pragma once

#include "analyzer.hpp"

namespace wa {

class TreeHeightBalancing : public IAnalyzer {
public:
  explicit TreeHeightBalancing(std::shared_ptr<AnalyzerContext> const &context) : IAnalyzer(context) {}

private:
  virtual void analyze_impl(Module &module);
};

} // namespace wa
