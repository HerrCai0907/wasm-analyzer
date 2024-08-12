#include "dom_builder.hpp"
#include "adt/dyn_bit_set.hpp"
#include "analyzer.hpp"
#include "basic_block_builder.hpp"
#include "cfg.hpp"
#include "debug.hpp"
#include <algorithm>
#include <cstddef>
#include <iostream>
#include <map>
#include <ranges>
#include <set>

namespace wa {

static auto get_dom(Cfg const &cfg) -> std::map<size_t, DynBitSet> {
  // prepare
  std::map<size_t, DynBitSet> dom_bit_maps{};
  size_t const bit_size = std::ranges::max(cfg.m_blocks | std::views::keys) + 1U;
  for (auto const &[index, block] : cfg.m_blocks) {
    if (index == EnterBlockIndex) {
      dom_bit_maps[index] = DynBitSet{bit_size};
    } else {
      dom_bit_maps[index] = ~DynBitSet{bit_size};
    }
  }
  std::map<size_t, std::set<size_t>> const &pred_map = cfg.get_pred_map();

  // iterator until no change
  bool is_changed = true;
  while (is_changed) {
    is_changed = false;
    for (auto const &[index, block] : cfg.m_blocks) {
      std::set<size_t> const &pred_set = pred_map.at(index);
      DynBitSet tmp = pred_set.empty() ? DynBitSet{bit_size} : ~DynBitSet{bit_size};
      for (size_t pred : pred_set) {
        tmp &= dom_bit_maps[pred];
      }
      tmp.mask(index);
      if (tmp != dom_bit_maps[index]) {
        is_changed = true;
        dom_bit_maps[index] = tmp;
      }
    }
  }

  if (Debug::is_debug_mode()) {
    for (auto const &[index, dom_bit_set] : dom_bit_maps) {
      std::cout << "dom of block[" << index << "]: " << dom_bit_set << "\n";
    }
  }
  return dom_bit_maps;
}

void DomBuilder::analyze_impl(Module &module) {
  auto cfg_builder = get_context()->m_analysis_manager->get_analyzer<BasicBlockBuilder>();
  cfg_builder->analyze(module);

  for (Cfg const &cfg : cfg_builder->get_cfgs()) {
    m_dom_bit_set = get_dom(cfg);
  }
}

std::shared_ptr<IAnalyzer> createDomBuilderAnalyzer(std::shared_ptr<AnalyzerContext> context) {
  return std::shared_ptr<DomBuilder>(new DomBuilder(context));
}

} // namespace wa
