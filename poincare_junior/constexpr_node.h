#ifndef POINCARE_CONSTEXPR_NODE_H
#define POINCARE_CONSTEXPR_NODE_H

#include "expressions/expressions.h"
#include "node.h"
#include "type_block.h"

namespace Poincare {

#warning Ensure that we can't use it in a non-constexpr mode
// TODO Use consteval? We should ensure one way or another that the number of specialized class is limited and used for constexpr methods only. Meaning that no code should be emited at runtime...

template <unsigned N>
class Tree {
public:
  constexpr Tree() {}
  constexpr Block * blockAtIndex(size_t i) { return &m_blocks[i]; }
  constexpr operator Node() const { return Node(const_cast<TypeBlock *>(m_blocks)); }
private:
  // Using this instead of a Block[N] simplifies up casting in constexprs
  TypeBlock m_blocks[N];
};

typedef bool (*BlockCreator)(Block *, size_t, uint8_t);

template<unsigned NumberOfBlocksInNode, unsigned ...Len>
constexpr static auto MakeTree(BlockCreator blockCreator, const Tree<Len> (&...nodes)) {
  // Compute the total length of the children
  constexpr unsigned k_numberOfChildren = sizeof...(Len);
  constexpr unsigned k_numberOfChildrenBlocks = (... + Len);

  Tree<k_numberOfChildrenBlocks + NumberOfBlocksInNode> tree;
  size_t i = 0;
  while (!blockCreator(tree.blockAtIndex(i), i, k_numberOfChildren)) {
    i++; // TODO factorize
  }

  size_t blockIndex = NumberOfBlocksInNode;
  for (Node node : {static_cast<Node>(nodes)...}) {
    // We can't use node.copyTreeTo(tree.blockAtIndex(blockIndex++)) because memcpy isn't constexpr
    // TODO: use constexpr version of memcpy in copyTreeTo?
    for (size_t i = 0; i < node.treeSize(); i++) {
      *(tree.blockAtIndex(blockIndex++)) = *(node.block() + i);
    }
  }
  return tree;
}

template<unsigned ...Len> static constexpr auto Add(const Tree<Len> (&...children)) { return MakeTree<Addition::k_numberOfBlocksInNode>(Addition::CreateBlockAtIndex, children...); }

template<unsigned L1, unsigned L2> static constexpr Tree<L1+L2+1> Div(const Tree<L1> child1, const Tree<L2> child2) { return MakeTree<Division::k_numberOfBlocksInNode>([](Block * b, size_t i, uint8_t nb) { return Division::CreateBlockAtIndex(b, i); }, child1, child2); }

template<unsigned ...Len> static constexpr auto Mult(const Tree<Len> (&...children)) { return MakeTree<Multiplication::k_numberOfBlocksInNode>(Multiplication::CreateBlockAtIndex, children...); }

template<unsigned L1, unsigned L2> static constexpr Tree<L1+L2+1> Pow(const Tree<L1> child1, const Tree<L2> child2) { return MakeTree<Power::k_numberOfBlocksInNode>([](Block * b, size_t i, uint8_t nb) { return Power::CreateBlockAtIndex(b, i); }, child1, child2); }

template<unsigned L1, unsigned L2> static constexpr Tree<L1+L2+1> Sub(const Tree<L1> child1, const Tree<L2> child2) { return MakeTree<Subtraction::k_numberOfBlocksInNode>([](Block * b, size_t i, uint8_t nb) { return Subtraction::CreateBlockAtIndex(b, i); }, child1, child2); }

static constexpr Tree<Constant::k_numberOfBlocksInNode> operator "" _n(char16_t name) {
  Tree<Constant::k_numberOfBlocksInNode> result;
  size_t i = 0;
  while (!Constant::CreateBlockAtIndex(result.blockAtIndex(i), i, name)) {
    i++; // TODO factorize
  }
  return result;
}

static constexpr Tree<3> operator "" _n(unsigned long long value) {
  assert(value != 0 && value != -1 && value != 1 && value != 2); // TODO: make this robust
  assert(value < 128); // TODO: handle negative small numbers?
  Tree<3> tree;
  size_t i = 0;
  while (!IntegerShort::CreateBlockAtIndex(tree.blockAtIndex(i), i, value)) {
    i++; // TODO factorize
  }
  return tree;
}

}

#endif
