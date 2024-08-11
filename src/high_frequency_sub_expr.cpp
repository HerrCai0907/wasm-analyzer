#include "high_frequency_sub_expr.hpp"
#include "adt/range.hpp"
#include "adt/string.hpp"
#include "analyzer.hpp"
#include "args.hpp"
#include "basic_block_builder.hpp"
#include "cfg.hpp"
#include "module.hpp"
#include <iostream>
#include <memory>
#include <optional>
#include <queue>
#include <stdexcept>
#include <vector>

namespace wa {

static const Arg<size_t> depth{"--HighFrequencySubExpr.depth", 16u};
static const Arg<size_t> statistic_num{"--HighFrequencySubExpr.num", 128u};

void HighFrequencySubExpr::analyze_impl(Module &module) {
  auto cfg_builder = get_context()->m_analysis_manager->get_analyzer<BasicBlockBuilder>();
  cfg_builder->analyze(module);

  for (BasicBlock const &block : cfg_builder->get_all_blocks()) {
    m_total_instr_num += block.m_instr.size();
    std::vector<InstrCode> codes{};
    for (Instr const *instr : block.m_instr) {
      codes.push_back(instr->get_code());
      for (size_t i = codes.size() > depth ? (codes.size() - depth) : 0U; i < codes.size(); i++) {
        m_trie.update(std::span<InstrCode>{&codes[i], codes.size() - i}, [](std::optional<size_t> &v) -> void {
          if (v.has_value()) {
            v.value()++;
          } else {
            v = 1;
          }
        });
      }
    }
  }
}

void HighFrequencySubExpr::dump_result() {
  if (m_total_instr_num == 0) {
    throw std::runtime_error("empty code section");
  }
  struct CountAndPath {
    size_t m_count;
    std::vector<InstrCode> m_path;
    bool operator<(CountAndPath const &o) const { return m_count < o.m_count; }
  };
  std::priority_queue<CountAndPath> results{};
  m_trie.for_each([&results](std::vector<InstrCode> path, size_t const &count) {
    results.push(CountAndPath{.m_count = count, .m_path = std::move(path)});
  });
  for (size_t i : Range(statistic_num)) {
    if (results.empty()) {
      break;
    }
    CountAndPath const &result = results.top();

    std::cout << StringOperator::join(result.m_path, ", ") << ": "
              << (static_cast<double>(result.m_count) / static_cast<double>(m_total_instr_num) * 100) << "%\n";
    results.pop();
  }
}

std::shared_ptr<IAnalyzer> createHighFrequencySubExprAnalyzer(std::shared_ptr<AnalyzerContext> context) {
  return std::shared_ptr<HighFrequencySubExpr>(new HighFrequencySubExpr(context));
}

} // namespace wa
