#pragma once

#include "module.hpp"
#include <memory>
#include <utility>
#include <vector>

namespace wa {

class AnalyzerContext {};

class IAnalyzer {
  std::shared_ptr<AnalyzerContext> m_context;

public:
  explicit IAnalyzer(std::shared_ptr<AnalyzerContext> const &context) : m_context(context) {}

  virtual void analyze(Module &module) = 0;
  virtual ~IAnalyzer() = default;

protected:
  std::shared_ptr<AnalyzerContext> getContext() { return m_context; }
};

#define ANALYZER(name) std::shared_ptr<IAnalyzer> create##name##Analyzer(std::shared_ptr<AnalyzerContext> context);
#include "analyzer_name.ins"
#undef ANALYZER

class AnalyzerManager {
  Module m_module;
  std::vector<std::shared_ptr<IAnalyzer>> m_analyzers;

public:
  AnalyzerManager(Module module) : m_module(std::move(module)), m_analyzers{} {}

  void registerAnalyzer(std::shared_ptr<IAnalyzer> analyzer) { m_analyzers.push_back(analyzer); }

  void analyze();
};

} // namespace wa
