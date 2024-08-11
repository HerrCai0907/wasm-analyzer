#include "cfg.hpp"
#include <cstddef>
#include <iostream>

namespace wa {

void Cfg::dump(std::map<size_t, BasicBlock> const &blocks) {
  std::cout << "Function CFG" << "\n";
  for (auto const &[block_index, block] : blocks) {
    std::cout << "  BB[" << block_index << "] -> ";
    for (size_t target : block.m_backs)
      std::cout << "BB[" << target << "] ";
    std::cout << "\n";
    for (Instr *instr : block.m_instr)
      std::cout << "    " << *instr << "\n";
  }
}

void BasicBlock::dump() const {
  std::cout << "target: ";
  for (size_t target : m_backs)
    std::cout << "BB[" << target << "] ";
  std::cout << "\n";
  for (Instr *instr : m_instr)
    std::cout << "  " << *instr << "\n";
}

void ExtendBasicBlock::dump() const {
  std::cout << "first: BB[" << m_first << "]\n";
  std::cout << "  set: ";
  for (size_t index : m_blocks) {
    std::cout << "BB[" << index << "] ";
  }
  std::cout << "\n";
};

BlockIterator &BlockIterator::operator++() {
  ++m_block_it;
  if (m_block_it != m_cfg_it->m_blocks.end()) {
    return *this;
  }
  while (true) {
    ++m_cfg_it;
    if (m_cfg_it == m_cfg_end) {
      m_block_it = InnerIt{};
      return *this;
    }
    if (m_cfg_it->m_blocks.empty()) {
      continue;
    }
    break;
  }
  m_block_it = m_cfg_it->m_blocks.begin();
  return *this;
}

} // namespace wa
