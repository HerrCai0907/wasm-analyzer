#pragma once

#include <cassert>
#include <vector>

namespace wa {

constexpr size_t tree_node_invalid_value = -1;

template <class T> struct TreeNode {
  T m_value;
  size_t m_parent = tree_node_invalid_value;
  size_t m_l = tree_node_invalid_value;
  size_t m_r = tree_node_invalid_value;

  bool has_children() const { return m_l != tree_node_invalid_value || m_r != tree_node_invalid_value; }
};

template <class T> class BinaryTree {
  std::vector<TreeNode<T>> m_nodes{};

public:
  constexpr size_t get_root() const { return 0U; }
  size_t create_root(T const &value) {
    assert(m_nodes.empty());
    m_nodes.push_back(TreeNode<T>{.m_value = value});
    return 0U;
  }
  enum class Direction { L, R };
  size_t create_node(T const &value, size_t parent, Direction direction) {
    size_t const new_index = m_nodes.size();
    switch (direction) {
    case Direction::L:
      assert(m_nodes.at(parent).m_l == tree_node_invalid_value);
      m_nodes.at(parent).m_l = new_index;
      break;
    case Direction::R:
      assert(m_nodes.at(parent).m_r == tree_node_invalid_value);
      m_nodes.at(parent).m_r = new_index;
      break;
    }
    m_nodes.push_back(TreeNode<T>{.m_value = value, .m_parent = parent});
    return new_index;
  }
  TreeNode<T> &at(size_t index) { return m_nodes.at(index); }
  TreeNode<T> const &at(size_t index) const { return m_nodes.at(index); }

  T &get_value(size_t index) { return at(index).m_value; }
  T const &get_value(size_t index) const { return at(index).m_value; }

  bool has_children(size_t index) const {
    TreeNode<T> const &node = at(index);
    return node.has_children();
  }

  void link(size_t parent, size_t child, Direction direction) {
    switch (direction) {
    case Direction::L:
      m_nodes.at(parent).m_l = child;
      break;
    case Direction::R:
      m_nodes.at(parent).m_r = child;
      break;
    }
    m_nodes.at(child).m_parent = parent;
  }

  std::vector<TreeNode<T>> const &get_nodes() const { return m_nodes; }
};

} // namespace wa
