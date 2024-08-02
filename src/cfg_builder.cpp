#include "cfg_builder.hpp"
#include "analyzer.hpp"
#include "cfg.hpp"
#include "error.hpp"
#include "module.hpp"
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <iostream>
#include <map>
#include <memory>
#include <ranges>
#include <set>
#include <vector>

namespace wa {

constexpr size_t EnterBlockIndex = 0;
constexpr size_t ExitBlockIndex = 1;

namespace {
class IWasmBlock {
  size_t m_br_target_block_index;
  size_t m_end_target_block_index;

public:
  explicit IWasmBlock(size_t br_target_block_index, size_t end_target_block_index)
      : m_br_target_block_index(br_target_block_index), m_end_target_block_index(end_target_block_index) {}
  size_t get_br_target_block_index() const { return m_br_target_block_index; }
  size_t get_end_target_block_index() const { return m_end_target_block_index; }
  virtual ~IWasmBlock() = default;
};
class WasmIfBlock : public IWasmBlock {
  size_t m_last_block_index;

public:
  explicit WasmIfBlock(size_t last_block_index, size_t next_block_index)
      : IWasmBlock(next_block_index, next_block_index), m_last_block_index(last_block_index) {}
  size_t get_last_block_index() const { return m_last_block_index; }
};
class WasmFuncBlock : public IWasmBlock {
public:
  explicit WasmFuncBlock(size_t next_block_index) : IWasmBlock(next_block_index, next_block_index) {}
};
class WasmBlockBlock : public IWasmBlock {
public:
  explicit WasmBlockBlock(size_t next_block_index) : IWasmBlock(next_block_index, next_block_index) {}
};
class WasmLoopBlock : public IWasmBlock {
public:
  explicit WasmLoopBlock(size_t this_block_index, size_t next_block_index)
      : IWasmBlock(this_block_index, next_block_index) {}
};
class CfgBuilderImpl {
  AnalyzerContext const *m_context;
  std::shared_ptr<Function> m_fn;
  size_t m_blocks_index_counter = std::max(EnterBlockIndex, ExitBlockIndex);
  std::map<size_t, BasicBlock> m_blocks{};
  size_t m_current_block_index = 0U;
  std::vector<std::unique_ptr<IWasmBlock>> m_wasm_block_stack{};

public:
  explicit CfgBuilderImpl(AnalyzerContext const *context, std::shared_ptr<Function> const &fn)
      : m_context(context), m_fn(fn) {}

  Cfg get() {
    build();
    simplify();
    return {.m_blocks = std::move(m_blocks)};
  }

private:
  void build();
  size_t append_block();
  void push_instr(size_t block_index, Instr *instr) { m_blocks[block_index].m_instr.push_back(instr); }
  void connect_block(size_t front, size_t back) { m_blocks.at(front).m_backs.insert(back); }
  void simplify();
  bool clean_block_no_instr_one_target();
};
size_t CfgBuilderImpl::append_block() {
  m_blocks_index_counter++;
  m_blocks.insert_or_assign(m_blocks_index_counter, BasicBlock{});
  return m_blocks_index_counter;
}
void CfgBuilderImpl::build() {
  {
    m_blocks.insert_or_assign(EnterBlockIndex, BasicBlock{});
    m_blocks.insert_or_assign(ExitBlockIndex, BasicBlock{});
    m_wasm_block_stack.push_back(std::make_unique<WasmFuncBlock>(ExitBlockIndex));
  }
  for (Instr &instr : m_fn->get_instr()) {
    switch (instr.get_code()) {
    case InstrCode::BLOCK: {
      size_t const this_block_index = append_block();
      size_t const next_block_index = append_block();
      connect_block(m_current_block_index, this_block_index);

      m_current_block_index = this_block_index;
      m_wasm_block_stack.push_back(std::make_unique<WasmBlockBlock>(next_block_index));
      break;
    }
    case InstrCode::LOOP: {
      size_t const this_block_index = append_block();
      size_t const next_block_index = append_block();
      connect_block(m_current_block_index, this_block_index);

      m_current_block_index = this_block_index;
      m_wasm_block_stack.push_back(std::make_unique<WasmLoopBlock>(m_current_block_index, next_block_index));
      break;
    }
    case InstrCode::IF: {
      size_t const last_block_index = m_current_block_index;
      size_t const then_block_index = append_block();
      size_t const next_block_index = append_block();
      connect_block(m_current_block_index, then_block_index);

      push_instr(m_current_block_index, &instr);
      m_current_block_index = then_block_index;
      m_wasm_block_stack.push_back(std::make_unique<WasmIfBlock>(last_block_index, next_block_index));
      break;
    }
    case InstrCode::ELSE: {
      WasmIfBlock *if_block = dynamic_cast<WasmIfBlock *>(m_wasm_block_stack.back().get());
      assert(if_block != nullptr);
      size_t const else_block_index = append_block();
      size_t const last_block_index = if_block->get_last_block_index();
      size_t const next_block_index = if_block->get_end_target_block_index();
      connect_block(m_current_block_index, next_block_index); // then to next
      connect_block(last_block_index, else_block_index);      // last to else

      m_current_block_index = else_block_index;
      break;
    }
    case InstrCode::END: {
      assert(!m_wasm_block_stack.empty());
      size_t const target_block_index = m_wasm_block_stack.back()->get_end_target_block_index();
      connect_block(m_current_block_index, target_block_index);
      WasmIfBlock *if_block = dynamic_cast<WasmIfBlock *>(m_wasm_block_stack.back().get());
      if (if_block != nullptr) {
        size_t const last_block_index = if_block->get_last_block_index();
        connect_block(last_block_index, target_block_index);
      }

      m_current_block_index = target_block_index;
      m_wasm_block_stack.pop_back();
      break;
    }
    case InstrCode::UNREACHABLE:
    case InstrCode::RETURN: {
      size_t const this_block_index = append_block();
      size_t const target_block_index = m_wasm_block_stack.front()->get_end_target_block_index();
      connect_block(m_current_block_index, target_block_index);

      push_instr(m_current_block_index, &instr);
      m_current_block_index = this_block_index;
      break;
    }
    case InstrCode::BR: {
      size_t const next_block_index = append_block();
      size_t const target_block_index =
          m_wasm_block_stack.at(m_wasm_block_stack.size() - 1 - instr.get_index())->get_br_target_block_index();
      connect_block(m_current_block_index, target_block_index);

      push_instr(m_current_block_index, &instr);
      m_current_block_index = next_block_index;
      break;
    }
    case InstrCode::BR_IF: {
      size_t const next_block_index = append_block();
      size_t const target_block_index =
          m_wasm_block_stack.at(m_wasm_block_stack.size() - 1 - instr.get_index())->get_br_target_block_index();
      connect_block(m_current_block_index, next_block_index);
      connect_block(m_current_block_index, target_block_index);

      push_instr(m_current_block_index, &instr);
      m_current_block_index = next_block_index;
      break;
    }
    case InstrCode::BR_TABLE: {
      throw Todo{"br_table"};
    }
    default: {
      push_instr(m_current_block_index, &instr);
      break;
    }
    }
  }
  assert(m_wasm_block_stack.empty());
}
bool CfgBuilderImpl::clean_block_no_instr_one_target() {
  std::map<size_t, size_t> replaced_blocks{};
  for (auto &[block_index, block] : m_blocks) {
    if (block.m_instr.empty() && block.m_backs.size() == 1) {
      replaced_blocks.insert_or_assign(block_index, *block.m_backs.begin());
    }
  }
  bool const isChanged = !replaced_blocks.empty();
  for (auto &[old_block, new_block] : replaced_blocks) {
    while (replaced_blocks.contains(new_block)) {
      new_block = replaced_blocks[new_block];
    }
  }
  auto const replacer = std::views::transform([&replaced_blocks](size_t target_block_index) -> size_t {
    if (replaced_blocks.contains(target_block_index)) {
      return replaced_blocks[target_block_index];
    }
    return target_block_index;
  });
  for (auto &[_, block] : m_blocks) {
    block.m_backs = block.m_backs | replacer | std::ranges::to<std::set>();
  }
  for (auto &[old_block, _] : replaced_blocks) {
    m_blocks.erase(old_block);
  }
  return isChanged;
}

void CfgBuilderImpl::simplify() {
  bool isChanged = true;
  size_t cnt = 0;
  while (isChanged) {
    if (m_context->m_options.is_enabled("--debug")) {
      std::cout << "=============== simplify " << cnt++ << " ===============\n";
      Cfg::dump(m_blocks);
    }

    isChanged = clean_block_no_instr_one_target();
  }
  if (m_context->m_options.is_enabled("--debug")) {
    std::cout << "============= simplify finish =============\n";
  }
}

} // namespace

void CfgBuilder::analyze_impl(Module &module) {
  m_cfg = module.m_functions |
          std::views::filter([](std::shared_ptr<Function> const &fn) { return !fn->is_import(); }) |
          std::views::transform(
              [this](std::shared_ptr<Function> const &fn) { return CfgBuilderImpl{get_context(), fn}.get(); }) |
          std::ranges::to<std::vector>();
}

std::shared_ptr<IAnalyzer> createCfgBuilderAnalyzer(std::shared_ptr<AnalyzerContext> context) {
  return std::make_shared<CfgBuilder>(context);
}

} // namespace wa
