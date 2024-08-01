#pragma once

#include "concept.hpp"
#include "module.hpp"
#include <map>
#include <memory>
#include <utility>

namespace wa {

class AnalyzerManager;

struct AnalyzerContext {
  AnalyzerManager *m_analysis_manager;
  explicit AnalyzerContext(AnalyzerManager &analysis_manager) : m_analysis_manager(&analysis_manager) {}
};

class IAnalyzer {
  bool m_is_finished = false;
  std::shared_ptr<AnalyzerContext> m_context;

public:
  explicit IAnalyzer(std::shared_ptr<AnalyzerContext> const &context) : m_context(context) {}

  virtual ~IAnalyzer() = default;

  void analyze(Module &module);

  size_t get_type_hash() { return typeid(*this).hash_code(); }

protected:
  virtual void analyze_impl(Module &module) = 0;

  std::shared_ptr<AnalyzerContext> get_context() { return m_context; }
  bool is_finished() const { return m_is_finished; }
};

#define ANALYZER(name) std::shared_ptr<IAnalyzer> create##name##Analyzer(std::shared_ptr<AnalyzerContext> context);
#include "analyzer_name.inc"

class AnalyzerManager {
  Module m_module;

  std::map<size_t, std::shared_ptr<IAnalyzer>> m_analyzers{};

public:
  AnalyzerManager(Module module) : m_module(std::move(module)), m_analyzers{} {}

  void register_analyzer(std::shared_ptr<IAnalyzer> analyzer) {
    m_analyzers.insert_or_assign(analyzer->get_type_hash(), analyzer);
  }

  template <Derived<IAnalyzer> T> std::shared_ptr<T> get_analyzer() {
    return std::dynamic_pointer_cast<T>(m_analyzers.at(typeid(T).hash_code()));
  }

  void analyze();
};

} // namespace wa
