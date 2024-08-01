#pragma once

#include "module.hpp"
#include <cstddef>
#include <map>
#include <set>
#include <vector>

namespace wa {

struct BasicBlock {
  std::vector<Instr *> m_instr{};
  std::set<size_t> m_backs{};
};

struct Cfg {
  std::map<size_t, BasicBlock> m_blocks{};

  static void dump(std::map<size_t, BasicBlock> const &blocks);
  void dump() const { dump(m_blocks); }
};

} // namespace wa
