#pragma once

#include "analyzer.hpp"
#include "cfg.hpp"
#include "module.hpp"
#include <memory>
#include <vector>

namespace wa {

class BasicBlockBuilder : public IAnalyzer {
  std::vector<Cfg> m_cfg{};

public:
  explicit BasicBlockBuilder(std::shared_ptr<AnalyzerContext> const &context) : IAnalyzer(context) {}
  std::vector<Cfg> const &get_cfgs() const { return m_cfg; };

  BlockRange get_all_blocks() const {
    return BlockRange{.m_begin = BlockIterator::create_begin(m_cfg), .m_end = BlockIterator::create_end(m_cfg)};
  }

private:
  void analyze_impl(Module &module) override;
};

} // namespace wa
