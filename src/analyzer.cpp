#include "analyzer.hpp"

namespace wa {

void AnalyzerManager::analyze() {
  for (auto &analyzer : m_analyzers) {
    analyzer->analyze(m_module);
  }
}

} // namespace wa
