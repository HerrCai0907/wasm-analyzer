#include "extend_basic_block_builder.hpp"
#include "analyzer.hpp"
#include "basic_block_builder.hpp"
#include "cfg.hpp"
#include "debug.hpp"
#include <cassert>
#include <cstddef>
#include <iostream>
#include <memory>
#include <vector>

namespace wa {

static std::map<size_t, size_t> get_front_block_num_map(Cfg const &cfg) {
  std::map<size_t, size_t> front_block_num_map{};
  for (auto const &[_, block] : cfg.m_blocks) {
    for (size_t back : block.m_backs) {
      if (front_block_num_map.contains(back)) {
        front_block_num_map[back]++;
      } else {
        front_block_num_map[back] = 1;
      }
    }
  }
  return front_block_num_map;
}

static bool is_first_block(size_t index, std::map<size_t, size_t> const &front_block_num_map) {
  // only the first basic block can have multiple predecessor basic blocks;
  return !front_block_num_map.contains(index) || (front_block_num_map.at(index) != 1);
}

static ExtendBasicBlock create_extend_basic_bloc(size_t index, std::map<size_t, BasicBlock> const &blocks,
                                                 std::map<size_t, size_t> const &front_block_num_map) {
  ExtendBasicBlock extend_block{.m_first = index, .m_blocks = {index}};
  std::vector<BasicBlock const *> work_list{&blocks.at(index)};
  while (!work_list.empty()) {
    BasicBlock const *current_block = work_list.back();
    work_list.pop_back();
    for (size_t back_index : current_block->m_backs) {
      if (!is_first_block(back_index, front_block_num_map)) {
        auto insert_result = extend_block.m_blocks.insert(back_index);
        if (insert_result.second) {
          // avoid circular dependencies caused infinite loop
          work_list.push_back(&blocks.at(back_index));
        }
      }
    }
  }
  return extend_block;
}

void ExtendBasicBlockBuilder::analyze_impl(Module &module) {
  std::shared_ptr<BasicBlockBuilder> cfg_builder = get_context()->m_analysis_manager->get_analyzer<BasicBlockBuilder>();
  cfg_builder->analyze(module);

  for (Cfg const &cfg : cfg_builder->get_cfgs()) {
    if (Debug::is_debug_mode()) {
      std::cout << "============= ExtendBasicBlock start =============\n";
    }
    m_extend_cfgs.push_back(ExtendCfg{});
    std::vector<ExtendBasicBlock> &current_extend_blocks = m_extend_cfgs.back().m_extend_blocks;
    std::map<size_t, size_t> const front_block_num_map = get_front_block_num_map(cfg);
    for (auto const &[index, _] : cfg.m_blocks) {
      if (!is_first_block(index, front_block_num_map)) {
        continue;
      }
      // only the first basic block can have multiple predecessor basic blocks;
      current_extend_blocks.push_back(create_extend_basic_bloc(index, cfg.m_blocks, front_block_num_map));
      if (Debug::is_debug_mode()) {
        current_extend_blocks.back().dump();
      }
    }
    if (Debug::is_debug_mode()) {
      std::cout << "============= ExtendBasicBlock end =============\n";
    }
  }
}

std::shared_ptr<IAnalyzer> createExtendBasicBlockBuilderAnalyzer(std::shared_ptr<AnalyzerContext> context) {
  return std::shared_ptr<ExtendBasicBlockBuilder>(new ExtendBasicBlockBuilder(context));
}

} // namespace wa
