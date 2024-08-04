#pragma once

#include "adt/trie.hpp"
#include "analyzer.hpp"
#include "module.hpp"
#include <memory>

namespace wa {

class HighFrequencySubExpr : public IAnalyzer {
  size_t m_depth = 0;
  size_t m_statistic_num = 0;
  size_t m_total_instr_num = 0;
  Trie<InstrCode, size_t> m_trie{};

public:
  explicit HighFrequencySubExpr(std::shared_ptr<AnalyzerContext> context) : IAnalyzer(context) {}
  void dump_result();

private:
  void load_options() override;
  void analyze_impl(Module &module) override;
};

} // namespace wa
