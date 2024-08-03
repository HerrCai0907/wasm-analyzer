#pragma once

#include "../concept.hpp"
#include <map>
#include <memory>
#include <optional>
#include <span>
#include <stdexcept>
#include <vector>

namespace wa {

template <class K, class V> class Trie {
  struct E {
    std::map<K, E *> m_sub_node_map{};
    std::optional<V> m_value = std::nullopt;
  };
  std::vector<std::unique_ptr<E>> m_nodes{};
  E *m_root = nullptr;

public:
  Trie() { m_root = add_node(); }
  void insert_or_assign(std::span<K> k, V v) {
    E *current = force_raw_at(k);
    current->m_value = v;
  }

  template <Callable<void, std::optional<V> &> Fn> void update(std::span<K> k, Fn const &func) {
    E *current = force_raw_at(k);
    func(current->m_value);
  }
  bool contains(std::span<K> k) const { return raw_at(k) != nullptr; }
  std::optional<V> &at(std::span<K> k) {
    E *value = raw_at(k);
    if (value == nullptr) {
      throw std::out_of_range("trie");
    }
    return value->m_value;
  }
  std::optional<V> const &at(std::span<K> k) const { return const_cast<Trie *>(this)->at(k); }

  template <Callable<void, std::vector<K>, V const &> Fn> void for_each(Fn const &fn) const {
    std::vector<K> path{};
    for_each_impl(fn, path, m_root);
  }

private:
  E *add_node() {
    m_nodes.push_back(std::make_unique<E>());
    return m_nodes.back().get();
  }
  E *raw_at(std::span<K> k) const {
    E *current = m_root;
    for (K const &ke : k) {
      if (!current->m_sub_node_map.contains(ke)) {
        return nullptr;
      }
      current = current->m_sub_node_map[ke];
    }
    return current;
  }

  E *force_raw_at(std::span<K> k) {
    E *current = m_root;
    for (K const &ke : k) {
      if (!current->m_sub_node_map.contains(ke)) {
        E *next = add_node();
        current->m_sub_node_map.insert_or_assign(ke, next);
        current = next;
      } else {
        current = current->m_sub_node_map[ke];
      }
    }
    return current;
  }

  template <Callable<void, std::vector<K>, V const &> Fn>
  static void for_each_impl(Fn const &fn, std::vector<K> &path, E *current) {
    if (current == nullptr)
      return;
    if (current->m_value.has_value()) {
      fn(path, current->m_value.value());
    }
    size_t const path_size = path.size();
    for (auto const &[k, v] : current->m_sub_node_map) {
      path.push_back(k);
      for_each_impl(fn, path, v);
      path.pop_back();
    }
  }
};

} // namespace wa
