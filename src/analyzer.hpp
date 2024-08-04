#pragma once

#include "concept.hpp"
#include "module.hpp"
#include <cstddef>
#include <map>
#include <memory>
#include <set>

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

  AnalyzerContext const *get_context() const { return m_context.get(); }
  bool is_finished() const { return m_is_finished; }
};

#define ANALYZER(name) std::shared_ptr<IAnalyzer> create##name##Analyzer(std::shared_ptr<AnalyzerContext> context);
#include "analyzer_name.inc"

class AnalyzerManager {
  std::set<size_t> m_active_analyzers{};
  Module m_module;
  std::map<size_t, std::shared_ptr<IAnalyzer>> m_analyzers;
  std::shared_ptr<AnalyzerContext> m_context;

public:
  explicit AnalyzerManager(Module const &module);

  template <Derived<IAnalyzer> T> std::shared_ptr<T> get_analyzer() const {
    return std::dynamic_pointer_cast<T>(m_analyzers.at(typeid(T).hash_code()));
  }

  void analyze();

#define ANALYZER(name) static bool is_##name##_active();
#include "analyzer_name.inc"
};

} // namespace wa
