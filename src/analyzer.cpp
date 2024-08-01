#include "analyzer.hpp"

namespace wa {

void IAnalyzer::analyze(Module &module) {
  if (!m_is_finished) {
    analyze_impl(module);
    m_is_finished = true;
  }
}

void AnalyzerManager::analyze() {
  for (auto &[_, analyzer] : m_analyzers) {
    analyzer->analyze(m_module);
  }
}

} // namespace wa
