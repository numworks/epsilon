#ifndef POINCARE_CONSTEXPR_NODE_H
#define POINCARE_CONSTEXPR_NODE_H

#include "interfaces/interfaces.h"
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
  TypeBlock m_blocks[N];
};

typedef bool (*BlockCreator)(Block *, size_t, uint8_t);

template<unsigned NumberOfBlocksInNode, unsigned ...Len>
constexpr static auto MakeTree(BlockCreator blockCreator, const Tree<Len> (&...nodes)) {
  // Compute the total length of the children
  constexpr unsigned NumberOfChildren = (... + Len);

  Tree<NumberOfChildren + NumberOfBlocksInNode> tree;
  size_t i = 0;
  while (!blockCreator(tree.blockAtIndex(i), i, NumberOfChildren)) {
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

template<unsigned ...Len> static constexpr auto Addition(const Tree<Len> (&...children)) { return MakeTree<AdditionInterface::k_numberOfBlocksInNode>(AdditionInterface::CreateBlockAtIndex, children...); }

template<unsigned L1, unsigned L2> static constexpr Tree<L1+L2+1> Division(const Tree<L1> child1, const Tree<L2> child2) { return MakeTree<DivisionInterface::k_numberOfBlocksInNode>([](Block * b, size_t i, uint8_t nb) { return DivisionInterface::CreateBlockAtIndex(b, i); }, child1, child2); }

template<unsigned ...Len> static constexpr auto Multiplication(const Tree<Len> (&...children)) { return MakeTree<MultiplicationInterface::k_numberOfBlocksInNode>(MultiplicationInterface::CreateBlockAtIndex, children...); }

template<unsigned L1, unsigned L2> static constexpr Tree<L1+L2+1> Power(const Tree<L1> child1, const Tree<L2> child2) { return MakeTree<PowerInterface::k_numberOfBlocksInNode>([](Block * b, size_t i, uint8_t nb) { return PowerInterface::CreateBlockAtIndex(b, i); }, child1, child2); }

template<unsigned L1, unsigned L2> static constexpr Tree<L1+L2+1> Subtraction(const Tree<L1> child1, const Tree<L2> child2) { return MakeTree<SubtractionInterface::k_numberOfBlocksInNode>([](Block * b, size_t i, uint8_t nb) { return SubtractionInterface::CreateBlockAtIndex(b, i); }, child1, child2); }

static constexpr Tree<ConstantInterface::k_numberOfBlocksInNode> operator "" _n(char16_t name) {
  Tree<ConstantInterface::k_numberOfBlocksInNode> result;
  size_t i = 0;
  while (!ConstantInterface::CreateBlockAtIndex(result.blockAtIndex(i), i, name)) {
    i++; // TODO factorize
  }
  return result;
}

static constexpr Tree<IntegerInterface::k_minimalNumberOfBlocksInNode + 1> operator "" _n(unsigned long long value) {
  assert(value < 256); // TODO: larger values
  Tree<IntegerInterface::k_minimalNumberOfBlocksInNode + 1> result;
  size_t i = 0;
  while (!IntegerInterface::CreateBlockAtIndex(result.blockAtIndex(i), i, value)) {
    i++; // TODO factorize
  }
  return result;
}

}

#endif
