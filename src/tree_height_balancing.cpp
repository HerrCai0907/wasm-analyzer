#include "tree_height_balancing.hpp"
#include "adt/binary_tree.hpp"
#include "adt/range.hpp"
#include "adt/y_combinator.hpp"
#include "analyzer.hpp"
#include "basic_block_builder.hpp"
#include "cfg.hpp"
#include "debug.hpp"
#include "error.hpp"
#include "instruction.hpp"
#include "module.hpp"
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <memory>
#include <queue>
#include <ranges>
#include <set>
#include <stack>
#include <stdexcept>
#include <vector>

namespace wa {

namespace {

struct TreeVec {
  std::vector<Instr const *> m_instructions{};
};

struct TreeInfo {
  Instr const *m_instr;
  int32_t m_rank;
};

} // namespace

static void dump_tree(BinaryTree<TreeInfo> const &tree) {
  make_y_combinator([&tree](auto self, size_t index, size_t indent) -> void {
    TreeNode<TreeInfo> const &node = tree.at(index);
    bool const has_l = node.m_l != tree_node_invalid_value;
    bool const has_r = node.m_r != tree_node_invalid_value;
    for (size_t i : Range{indent})
      std::cout << "  ";
    std::cout << *node.m_value.m_instr << "\n";
    if (has_l)
      self(node.m_l, indent + 1);
    if (has_r)
      self(node.m_r, indent + 1);
  })(tree.get_root(), 0);

  size_t height = make_y_combinator([&tree](auto self, size_t index) -> size_t {
    TreeNode<TreeInfo> const &node = tree.at(index);
    bool const has_l = node.m_l != tree_node_invalid_value;
    bool const has_r = node.m_r != tree_node_invalid_value;
    return 1U + std::max(has_l ? self(node.m_l) : 0, has_r ? self(node.m_r) : 0);
  })(tree.get_root());
  std::cout << "tree height is " << height << "\n";
}
static auto transformer(TreeVec const &vec) -> BinaryTree<TreeInfo> {
  assert(!vec.m_instructions.empty());
  BinaryTree<TreeInfo> tree{};
  size_t const root = tree.create_root(TreeInfo{.m_instr = vec.m_instructions.back(), .m_rank = -1});
  struct StackElement {
    size_t m_missed_operand_count;
    size_t m_result_count;
    size_t m_tree_index;

    explicit StackElement(Instr const *instr, size_t tree_index)
        : m_missed_operand_count(instr->get_operand_count()), m_result_count(instr->get_result_count()),
          m_tree_index(tree_index) {}
  };
  std::stack<StackElement> missed_operand_count_stack{};
  missed_operand_count_stack.push(StackElement{vec.m_instructions.back(), root});
  for (Instr const *instr : vec.m_instructions | std::views::reverse | std::views::drop(1)) {
    auto const get_direction = [](size_t missed_operand_count) -> BinaryTree<TreeInfo>::Direction {
      switch (missed_operand_count) {
      case 1:
        return BinaryTree<TreeInfo>::Direction::R;
      case 2:
        return BinaryTree<TreeInfo>::Direction::L;
      default:
        throw std::runtime_error(__PRETTY_FUNCTION__);
      }
    };
    auto top = [&]() -> StackElement & { return missed_operand_count_stack.top(); };
    size_t const index = tree.create_node(TreeInfo{.m_instr = instr, .m_rank = -1}, top().m_tree_index,
                                          get_direction(top().m_missed_operand_count));
    top().m_missed_operand_count -= instr->get_result_count();
    while (!missed_operand_count_stack.empty() && top().m_missed_operand_count == 0) {
      missed_operand_count_stack.pop();
    }
    if (instr->get_operand_count() != 0) {
      missed_operand_count_stack.push(StackElement{instr, index});
    }
  }
  assert(missed_operand_count_stack.empty());
  if (Debug::is_debug_mode()) {
    std::cout << "========================= BEFORE TREE HEIGHT BALANCING =============================\n";
    dump_tree(tree);
    std::cout << "========================= ============================ =============================\n";
  }
  return tree;
}

static auto convert(BasicBlock const &block) -> std::vector<BinaryTree<TreeInfo>> {
  // FIXME(full support)
  static const std::set<InstrCode> tree_node{// value
                                             InstrCode::GLOBAL_GET, InstrCode::I32_CONST,
                                             // operator
                                             InstrCode::I32_MUL, InstrCode::I32_ADD};
  std::vector<TreeVec> tree_vectors{};
  tree_vectors.push_back({});
  for (Instr const *instr : block.m_instr) {
    if (tree_node.contains(instr->get_code())) {
      tree_vectors.back().m_instructions.push_back(instr);
    } else {
      if (!tree_vectors.back().m_instructions.empty()) {
        tree_vectors.push_back({});
      }
    }
  }
  return tree_vectors | std::views::filter([](TreeVec const &vec) -> bool { return !vec.m_instructions.empty(); }) |
         std::views::transform(transformer) | std::ranges::to<std::vector>();
}

namespace {
struct ScalePriorityComparison {
  static bool operator()(size_t a, size_t b) {
    // To avoid nest recursive, here smaller scale root should be process firstly.
    // we assume index less means scale larger.
    bool const is_scale_a_larger_than_b = a < b;
    bool const is_swap_a_b = is_scale_a_larger_than_b;
    return is_swap_a_b;
  }
};
using NodeIndex = size_t;
using RootsQueue = std::priority_queue<NodeIndex, std::vector<NodeIndex>, ScalePriorityComparison>;
struct RankPriorityComparison {
  BinaryTree<TreeInfo> const &m_tree;
  static int32_t get_value_rank(size_t index, BinaryTree<TreeInfo> const &tree) {
    InstrCode code = tree.get_value(index).m_instr->get_code();
    if (code == InstrCode::I32_CONST)
      return 0;
    if (code == InstrCode::LOCAL_GET)
      return 1;
    if (code == InstrCode::GLOBAL_GET)
      return 2;
    throw Todo(__PRETTY_FUNCTION__);
  }
  int32_t get_rank(size_t index) const { return m_tree.get_value(index).m_rank; }
  bool operator()(size_t a, size_t b) const {
    bool const is_rank_a_larger_then_b = get_rank(a) > get_rank(b);
    bool const is_swap_a_b = is_rank_a_larger_then_b;
    return is_swap_a_b;
  }
};
using RankQueue = std::priority_queue<NodeIndex, std::vector<NodeIndex>, RankPriorityComparison>;
} // namespace

static bool is_root(BinaryTree<TreeInfo> const &tree, size_t index) {
  TreeNode<TreeInfo> const &node = tree.at(index);
  // FIXME operator should be commutative and associative
  return node.has_children() && tree.get_value(node.m_parent).m_instr->get_code() != node.m_value.m_instr->get_code();
}
static auto mark_root(BinaryTree<TreeInfo> const &tree) -> RootsQueue {
  RootsQueue roots{};
  roots.push(tree.get_root());
  for (size_t i : Range{1, tree.get_nodes().size()}) {
    if (is_root(tree, i))
      roots.push(i);
  }
  return roots;
}
static auto balance(size_t root_index, BinaryTree<TreeInfo> &tree) -> void;
static auto flatten(size_t node_index, RankQueue &rank_queue, BinaryTree<TreeInfo> &tree) -> std::set<size_t> {
  std::set<size_t> available_op_slot{};
  TreeNode<TreeInfo> &node = tree.at(node_index);
  if (!node.has_children()) {
    node.m_value.m_rank = RankPriorityComparison::get_value_rank(node_index, tree);
    rank_queue.push(node_index);
    // return node.m_value.m_rank;
  } else if (is_root(tree, node_index)) {
    balance(node_index, tree);
    rank_queue.push(node_index);
    // return node.m_value.m_rank;
  } else {
    assert(node.m_l != tree_node_invalid_value);
    assert(node.m_r != tree_node_invalid_value);
    available_op_slot.insert(node_index);
    available_op_slot.insert_range(flatten(node.m_l, rank_queue, tree));
    available_op_slot.insert_range(flatten(node.m_r, rank_queue, tree));
  }
  return available_op_slot;
}
static auto rebuild(size_t root_index, std::set<size_t> available_op_slot, RankQueue &rank_queue,
                    BinaryTree<TreeInfo> &tree) {
  while (true) {
    size_t l = rank_queue.top();
    rank_queue.pop();
    size_t r = rank_queue.top();
    rank_queue.pop();
    if (Debug::is_debug_mode()) {
      std::cout << "combine " << *tree.get_value(l).m_instr << " " << *tree.get_value(r).m_instr << "\n";
    }
    if (rank_queue.empty()) {
      tree.link(root_index, l, BinaryTree<TreeInfo>::Direction::L);
      tree.link(root_index, r, BinaryTree<TreeInfo>::Direction::R);
      tree.get_value(root_index).m_rank = tree.get_value(l).m_rank + tree.get_value(r).m_rank;
      return;
    } else {
      assert(!available_op_slot.empty());
      size_t operator_node_index = *available_op_slot.begin();
      available_op_slot.erase(operator_node_index);
      tree.link(operator_node_index, l, BinaryTree<TreeInfo>::Direction::L);
      tree.link(operator_node_index, r, BinaryTree<TreeInfo>::Direction::R);
      tree.get_value(operator_node_index).m_rank = tree.get_value(l).m_rank + tree.get_value(r).m_rank;
      rank_queue.push(operator_node_index);
    }
  }
}
static auto balance(size_t root_index, BinaryTree<TreeInfo> &tree) -> void {
  TreeNode<TreeInfo> &root_node = tree.at(root_index);
  if (root_node.m_value.m_rank >= 0)
    return;
  RankQueue rank_queue{RankPriorityComparison{.m_tree = tree}};
  std::set<size_t> available_op_slot{};
  available_op_slot.insert_range(flatten(root_node.m_l, rank_queue, tree));
  available_op_slot.insert_range(flatten(root_node.m_r, rank_queue, tree));
  rebuild(root_index, available_op_slot, rank_queue, tree);
}

void TreeHeightBalancing::analyze_impl(Module &module) {
  auto cfg_builder = get_context()->m_analysis_manager->get_analyzer<BasicBlockBuilder>();
  cfg_builder->analyze(module);
  for (BasicBlock const &block : cfg_builder->get_all_blocks()) {
    std::vector<BinaryTree<TreeInfo>> trees = convert(block);
    for (BinaryTree<TreeInfo> &tree : trees) {
      RootsQueue roots = mark_root(tree);
      while (!roots.empty()) {
        balance(roots.top(), tree);
        roots.pop();
      }
      if (Debug::is_debug_mode()) {
        std::cout << "========================== AFTER TREE HEIGHT BALANCING =============================\n";
        dump_tree(tree);
        std::cout << "========================== =========================== =============================\n";
      }
    }
  }
}

std::shared_ptr<IAnalyzer> createTreeHeightBalancingAnalyzer(std::shared_ptr<AnalyzerContext> context) {
  return std::shared_ptr<TreeHeightBalancing>(new TreeHeightBalancing(context));
}

} // namespace wa
