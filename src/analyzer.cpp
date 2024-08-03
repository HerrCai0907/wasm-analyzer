#include "analyzer.hpp"

namespace wa {

void IAnalyzer::analyze(Module &module) {
  if (!m_is_option_loaded) {
    load_options();
    m_is_option_loaded = true;
  }
  if (!m_is_finished) {
    analyze_impl(module);
    m_is_finished = true;
  }
}

void AnalyzerManager::analyze() {
  for (size_t const &hash : m_active_analyzers) {
    m_analyzers.at(hash)->analyze(m_module);
  }
}

AnalyzerManager::AnalyzerManager(AnalyzerOption const &options, Module const &module)
    : m_module(module), m_analyzers{}, m_context{new AnalyzerContext(*this, options)} {
  auto register_analyzer = [this](std::shared_ptr<IAnalyzer> analyzer) {
    m_analyzers.insert_or_assign(analyzer->get_type_hash(), analyzer);
  };

#define ANALYZER(name) register_analyzer(create##name##Analyzer(m_context));
#include "analyzer_name.inc"
}

} // namespace wa
