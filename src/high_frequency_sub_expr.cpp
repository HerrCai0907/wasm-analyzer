#include "high_frequency_sub_expr.hpp"
#include "analyzer.hpp"
#include "cfg.hpp"
#include "cfg_builder.hpp"
#include <iostream>
#include <memory>

namespace wa {

void HighFrequencySubExpr::analyze_impl(Module &module) {
  std::shared_ptr<CfgBuilder> cfg_builder = get_context()->m_analysis_manager->get_analyzer<CfgBuilder>();
  cfg_builder->analyze(module);

  const size_t depth = get_context()->m_options.get<size_t>("--high-frequency-sub-expr-depth").value_or(4u);
  std::cout << "depth = " << depth << "\n";
  for (Cfg const &cfg : cfg_builder->get_cfg()) {
    for (auto const &[_, block] : cfg.m_blocks) {
      for (size_t i = 0; i < block.m_instr.size(); i++) {
      }
    }
  }
}

std::shared_ptr<IAnalyzer> createHighFrequencySubExprAnalyzer(std::shared_ptr<AnalyzerContext> context) {
  return std::shared_ptr<HighFrequencySubExpr>(new HighFrequencySubExpr(context));
}

} // namespace wa
