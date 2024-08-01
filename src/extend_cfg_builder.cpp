#include "analyzer.hpp"
#include "cfg_builder.hpp"
#include <memory>

namespace wa {

class ExtendCfgBuilder : public IAnalyzer {
public:
  explicit ExtendCfgBuilder(std::shared_ptr<AnalyzerContext> context) : IAnalyzer(context) {}
  void analyze_impl(Module &module) override {
    std::shared_ptr<CfgBuilder> cfg_builder = get_context()->m_analysis_manager->get_analyzer<CfgBuilder>();
    cfg_builder->analyze(module);
    std::ranges::for_each(cfg_builder->get_cfg(), [](Cfg const &cfg) { cfg.dump(); });
  }
};

std::shared_ptr<IAnalyzer> createExtendCfgBuilderAnalyzer(std::shared_ptr<AnalyzerContext> context) {
  return std::shared_ptr<ExtendCfgBuilder>(new ExtendCfgBuilder(context));
}

} // namespace wa
