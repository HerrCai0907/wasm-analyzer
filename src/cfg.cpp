#include "cfg.hpp"
#include <iostream>

namespace wa {

void Cfg::dump(std::map<size_t, BasicBlock> const &blocks) {
  std::cout << "Function CFG" << "\n";
  for (auto const &[block_index, block] : blocks) {
    std::cout << "  BB[" << block_index << "] -> ";
    for (size_t target : block.m_backs)
      std::cout << "BB[" << target << "] ";
    std::cout << "\n";
    for (Instr *instr : block.m_instr) {
      std::cout << "    " << *instr << "\n";
    }
  }
}

} // namespace wa
