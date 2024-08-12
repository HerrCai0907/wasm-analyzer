#pragma once

#include "string.hpp"
#include <bitset>
#include <cassert>
#include <cstddef>
#include <ostream>
#include <string_view>
#include <vector>

namespace wa {

class DynBitSet {
  size_t m_bit_size = 0U;
  std::vector<std::bitset<64U>> m_raw{};

  static constexpr size_t block_bit_size = sizeof(decltype(m_raw)::value_type) * 8U;

  struct InnerIndex {
    size_t m_raw_index;
    size_t m_bit_index;
  };

  static InnerIndex convert_index(size_t index) {
    size_t raw_index = index / block_bit_size;
    size_t left = index - raw_index * block_bit_size;
    return {.m_raw_index = raw_index, .m_bit_index = left};
  }

public:
  DynBitSet() = default;
  explicit DynBitSet(size_t bit_size) : m_bit_size(bit_size), m_raw((bit_size + block_bit_size - 1) / block_bit_size) {}

  void mask(size_t index) {
    assert(index <= m_bit_size);
    InnerIndex inner_index = convert_index(index);
    m_raw[inner_index.m_raw_index].set(inner_index.m_bit_index, true);
  }
  void unmask(size_t index) {
    assert(index <= m_bit_size);
    InnerIndex inner_index = convert_index(index);
    m_raw[inner_index.m_raw_index].set(inner_index.m_bit_index, false);
  }
  void set(size_t index, bool value) {
    assert(index <= m_bit_size);
    InnerIndex inner_index = convert_index(index);
    m_raw[inner_index.m_raw_index].set(inner_index.m_bit_index, value);
  }

  DynBitSet operator~() const {
    DynBitSet new_bit_set{m_bit_size};
    for (size_t i = 0; i < m_raw.size(); i++)
      new_bit_set.m_raw[i] = ~m_raw[i];
    return new_bit_set;
  }
  DynBitSet operator|(DynBitSet const &o) const {
    assert(m_bit_size == o.m_bit_size);
    DynBitSet new_bit_set{m_bit_size};
    for (size_t i = 0; i < m_raw.size(); i++)
      new_bit_set.m_raw[i] = m_raw[i] | o.m_raw[i];
    return new_bit_set;
  }
  DynBitSet operator&(DynBitSet const &o) const {
    assert(m_bit_size == o.m_bit_size);
    DynBitSet new_bit_set{m_bit_size};
    for (size_t i = 0; i < m_raw.size(); i++)
      new_bit_set.m_raw[i] = m_raw[i] & o.m_raw[i];
    return new_bit_set;
  }

  DynBitSet &operator|=(DynBitSet const &o) {
    assert(m_bit_size == o.m_bit_size);
    for (size_t i = 0; i < m_raw.size(); i++)
      m_raw[i] |= o.m_raw[i];
    return *this;
  }
  DynBitSet &operator&=(DynBitSet const &o) {
    assert(m_bit_size == o.m_bit_size);
    for (size_t i = 0; i < m_raw.size(); i++)
      m_raw[i] &= o.m_raw[i];
    return *this;
  }

  bool operator==(DynBitSet const &o) const {
    assert(m_bit_size == o.m_bit_size);
    for (size_t i = 0; i < m_raw.size(); i++) {
      if (m_raw[i] != o.m_raw[i]) {
        return false;
      }
    }
    return true;
  }
  bool operator!=(DynBitSet const &o) const { return !operator==(o); }

  friend std::ostream &operator<<(std::ostream &os, DynBitSet const &bit_set) {
    std::string str = StringOperator::join(bit_set.m_raw, "");
    size_t drop_bit = bit_set.m_raw.size() * block_bit_size - bit_set.m_bit_size;
    return os << std::string_view{&str[drop_bit], str.size() - drop_bit};
  }
};

} // namespace wa
