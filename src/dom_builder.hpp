#pragma once

#include "adt/dyn_bit_set.hpp"
#include "analyzer.hpp"
#include <memory>

namespace wa {

class DomBuilder : public IAnalyzer {
  std::map<size_t, DynBitSet> m_dom_bit_set;

public:
  explicit DomBuilder(std::shared_ptr<AnalyzerContext> const &context) : IAnalyzer(context) {}

private:
  void analyze_impl(Module &module) override;
};

} // namespace wa
