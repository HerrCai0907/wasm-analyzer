#pragma once

#include "concept.hpp"
#include "module.hpp"
#include <any>
#include <cstddef>
#include <map>
#include <memory>
#include <set>
#include <string>

namespace wa {

class AnalyzerManager;

class AnalyzerOption {
  std::shared_ptr<std::map<std::string, std::any>> m_options{new std::map<std::string, std::any>()};

public:
  template <OptionValue T> void set(std::string const &key, T const &value) {
    m_options->insert_or_assign(key, std::any{value});
  }
  bool is_enabled(std::string const &key) const { return m_options->contains(key); }

  template <class T> std::optional<T> get(std::string const &key) const {
    if (!m_options->contains(key))
      return std::nullopt;
    std::istringstream ss{std::any_cast<const char *>(m_options->at(key))};
    T v;
    ss >> v;
    if (ss.fail())
      return std::nullopt;
    return v;
  }
};

struct AnalyzerContext {
  AnalyzerManager *m_analysis_manager;
  AnalyzerOption const m_options;
  explicit AnalyzerContext(AnalyzerManager &analysis_manager, AnalyzerOption const &options)
      : m_analysis_manager(&analysis_manager), m_options(options) {}
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
  AnalyzerManager(AnalyzerOption const &options, Module const &module);

  template <Derived<IAnalyzer> T> void active_analyzer() { m_active_analyzers.insert(typeid(T).hash_code()); }

  template <Derived<IAnalyzer> T> std::shared_ptr<T> get_analyzer() const {
    return std::dynamic_pointer_cast<T>(m_analyzers.at(typeid(T).hash_code()));
  }

  void analyze();
};

} // namespace wa
