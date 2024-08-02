#pragma once

#include <cstddef>
namespace wa {

class Range {
  std::size_t m_begin;
  std::size_t m_end;

public:
  Range(std::size_t end) : m_begin(0), m_end(end) {}

  class RangeIterator {
    std::size_t m_cnt;

  public:
    RangeIterator(std::size_t cnt) : m_cnt(cnt) {}

    size_t &operator*() { return m_cnt; }
    RangeIterator &operator++() {
      ++m_cnt;
      return *this;
    }
    bool operator!=(RangeIterator const &o) const { return m_cnt != o.m_cnt; }
    bool operator==(RangeIterator const &o) const { return m_cnt == o.m_cnt; }
  };

  RangeIterator begin() const { return m_begin; }
  RangeIterator end() const { return m_end; }
};

} // namespace wa
