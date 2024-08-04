#include "analyzer.hpp"
#include "args.hpp"

namespace wa {

#define ANALYZER(name) static Arg<bool> name##_active{"--" #name, false};
#include "analyzer_name.inc"

void IAnalyzer::analyze(Module &module) {
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

AnalyzerManager::AnalyzerManager(Module const &module)
    : m_module(module), m_analyzers{}, m_context{new AnalyzerContext(*this)} {
  auto register_analyzer = [this](std::shared_ptr<IAnalyzer> analyzer) {
    m_analyzers.insert_or_assign(analyzer->get_type_hash(), analyzer);
  };
  auto active_analyzer = [this](std::shared_ptr<IAnalyzer> analyzer) {
    m_active_analyzers.insert(analyzer->get_type_hash());
  };

#define ANALYZER(name)                                                                                                 \
  {                                                                                                                    \
    auto analyzer = create##name##Analyzer(m_context);                                                                 \
    register_analyzer(analyzer);                                                                                       \
    if (name##_active) {                                                                                               \
      active_analyzer(analyzer);                                                                                       \
    }                                                                                                                  \
  }
#include "analyzer_name.inc"
}

#define ANALYZER(name)                                                                                                 \
  bool AnalyzerManager::is_##name##_active() { return name##_active; }
#include "analyzer_name.inc"

} // namespace wa
