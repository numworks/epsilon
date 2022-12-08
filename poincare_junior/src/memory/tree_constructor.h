#ifndef POINCARE_MEMORY_TREE_CONSTRUCTOR_H
#define POINCARE_MEMORY_TREE_CONSTRUCTOR_H

#include "node_constructor.h"
#include "node.h"

namespace Poincare {

#warning Ensure that we can't use it in a non-constexpr mode
// TODO Use consteval? We should ensure one way or another that the number of specialized class is limited and used for constexpr methods only. Meaning that no code should be emited at runtime...


template <unsigned N>
class Tree {
public:
  constexpr Tree() {}
  constexpr Block & operator[] (size_t n) { return m_blocks[n]; }
  constexpr operator Node() const { return Node(const_cast<TypeBlock *>(m_blocks)); }
private:
  // Using this instead of a Block[N] simplifies up casting in constexprs
  TypeBlock m_blocks[N];
};

template<BlockType type, unsigned ...Len>
constexpr static auto MakeTree(const Tree<Len> (&...nodes)) {
  // Compute the total length of the children
  constexpr unsigned k_numberOfChildren = sizeof...(Len);
  constexpr unsigned k_numberOfChildrenBlocks = (0 + ... + Len);
  constexpr size_t numberOfBlocksInNode = TypeBlock::NumberOfMetaBlocks(type);

  Tree<k_numberOfChildrenBlocks + numberOfBlocksInNode> tree;
  CreateNode<type>(&tree, k_numberOfChildren);

  size_t blockIndex = numberOfBlocksInNode;
  size_t childIndex = 0;
  int childrenSizes[] = {Len...};
  std::initializer_list<Node> childrenNodes{static_cast<Node>(nodes)...};
  for (Node node : childrenNodes) {
    // We can't use node.copyTreeTo(tree.blockAtIndex(blockIndex++)) because memcpy isn't constexpr
    // TODO: use constexpr version of memcpy in copyTreeTo?
    for (size_t i = 0; i < childrenSizes[childIndex]; i++) {
      tree[blockIndex++] = *(node.block() + i);
    }
    childIndex++;
  }
  return tree;
}

template<unsigned ...Len> static constexpr auto Add(const Tree<Len> (&...children)) { return MakeTree<BlockType::Addition>(children...); }

template<unsigned L1, unsigned L2> static constexpr Tree<L1+L2+1> Div(const Tree<L1> child1, const Tree<L2> child2) { return MakeTree<BlockType::Division>(child1, child2); }

template<unsigned ...Len> static constexpr auto Mult(const Tree<Len> (&...children)) { return MakeTree<BlockType::Multiplication>(children...); }

template<unsigned ...Len> static constexpr auto Set(const Tree<Len> (&...children)) { return MakeTree<BlockType::Set>(children...); }

template<unsigned L1, unsigned L2> static constexpr Tree<L1+L2+1> Pow(const Tree<L1> child1, const Tree<L2> child2) { return MakeTree<BlockType::Power>(child1, child2); }

template<unsigned L1, unsigned L2> static constexpr Tree<L1+L2+1> Sub(const Tree<L1> child1, const Tree<L2> child2) { return MakeTree<BlockType::Subtraction>(child1, child2); }

template <BlockType blockType, unsigned N, typename... Types>
constexpr static void CreateNode(Tree<N> * tree, Types... args) {
  size_t i = 0;
  while (!NodeConstructor::CreateBlockAtIndexForType<blockType>(&(*tree)[i], i, args...)) {
    i++;
  }
}

constexpr Tree<TypeBlock::NumberOfMetaBlocks(BlockType::Constant)> operator "" _n(char16_t name) {
  Tree<TypeBlock::NumberOfMetaBlocks(BlockType::Constant)> tree;
  CreateNode<BlockType::Constant>(&tree, name);
  return tree;
}

// TODO: improve suffix format

constexpr Tree<1> operator "" _nsn(unsigned long long value) { // single-block node
  Tree<1> tree;
  switch (value) {
    case 1:
      CreateNode<BlockType::MinusOne>(&tree);
      break;
    default:
      assert(false);
  }
  return tree;
}

constexpr Tree<1> operator "" _sn(unsigned long long value) { // single-block node
  Tree<1> tree;
  switch (value) {
    case 0:
      CreateNode<BlockType::Zero>(&tree);
      break;
    case 1:
      CreateNode<BlockType::One>(&tree);
      break;
    case 2:
      CreateNode<BlockType::Two>(&tree);
      break;
    default:
      assert(false);
  }
  return tree;
}

constexpr Tree<3> operator "" _n(unsigned long long value) {
  assert(value != 0 && value != -1 && value != 1 && value != 2); // TODO: make this robust
  assert(value < 128); // TODO: handle negative small numbers?
  Tree<3> tree;
  CreateNode<BlockType::IntegerShort>(&tree, static_cast<int8_t>(value));
  return tree;
}

constexpr Tree<TypeBlock::NumberOfMetaBlocks(BlockType::Float)> operator "" _fn(long double value) {
  Tree<TypeBlock::NumberOfMetaBlocks(BlockType::Float)> tree;
  CreateNode<BlockType::Float>(&tree, static_cast<float>(value));
  return tree;
}

}

#endif
