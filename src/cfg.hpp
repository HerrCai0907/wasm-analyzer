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

  void dump() const;
};

struct Cfg {
  std::map<size_t, BasicBlock> m_blocks{};

  static void dump(std::map<size_t, BasicBlock> const &blocks);
  void dump() const { dump(m_blocks); }
};

struct ExtendBasicBlock {
  size_t m_first = -1;
  std::set<size_t> m_blocks{};

  void dump() const;
};

struct ExtendCfg {
  std::vector<ExtendBasicBlock> m_extend_blocks{};
};

} // namespace wa
