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

class BlockIterator {
  std::vector<Cfg>::const_iterator m_cfg_it{};
  std::vector<Cfg>::const_iterator m_cfg_end{};
  std::map<size_t, BasicBlock>::const_iterator m_block_it{};

  using InnerIt = std::map<size_t, BasicBlock>::const_iterator;

public:
  static BlockIterator create_begin(std::vector<Cfg> const &cfgs) {
    BlockIterator it{};
    it.m_cfg_it = cfgs.begin();
    it.m_cfg_end = cfgs.end();
    it.m_block_it = (cfgs.empty() ? InnerIt{} : cfgs.front().m_blocks.begin());
    return it;
  }
  static BlockIterator create_end(std::vector<Cfg> const &cfgs) {
    BlockIterator it{};
    it.m_cfg_it = cfgs.end();
    it.m_cfg_end = cfgs.end();
    return it;
  }

  using difference_type = std::ptrdiff_t;
  BasicBlock const &operator*() { return m_block_it->second; }
  BlockIterator &operator++();
  void operator++(int) { ++*this; }
  bool operator!=(BlockIterator const &o) const { return !this->operator==(o); }
  bool operator==(BlockIterator const &o) const { return m_cfg_it == o.m_cfg_it && m_block_it == o.m_block_it; }
};
static_assert(std::input_or_output_iterator<BlockIterator>, "");

struct BlockRange {
  BlockIterator m_begin;
  BlockIterator m_end;
  BlockIterator const &begin() { return m_begin; }
  BlockIterator const &end() { return m_end; }
};
static_assert(::std::ranges::range<BlockRange>, "");

struct ExtendBasicBlock {
  size_t m_first = -1;
  std::set<size_t> m_blocks{};

  void dump() const;
};

struct ExtendCfg {
  std::vector<ExtendBasicBlock> m_extend_blocks{};
};

} // namespace wa
