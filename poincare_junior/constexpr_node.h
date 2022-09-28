#ifndef POINCARE_CONSTEXPR_NODE_H
#define POINCARE_CONSTEXPR_NODE_H

#include "type_block.h"
#include "interfaces/interfaces.h"

namespace Poincare {

#warning Ensure that we can't use it in a non-constexpr mode
// TODO Use consteval? We should ensure one way or another that the number of specialized class is limited and used for constexpr methods only. Meaning that no code should be emited at runtime...

template <unsigned N>
class Tree {
public:
  constexpr Tree() {}
  constexpr Tree(Block * blocks) {
    memcpy(m_blocks, blocks, N * sizeof(Block));
  }
  constexpr operator Node() { return Node(static_cast<TypeBlock *>(m_blocks)); }
private:
  Block m_blocks[N];
};

typedef size_t (*NodeCreator)(Block *, size_t);
template<unsigned ...Len>
constexpr static auto MakeTree(NodeCreator nodeCreator, const Tree<Len> * (&...nodes)) {
  // Compute the total length of the children
  constexpr unsigned NumberOfChildren = (... + Len);

  Block blocks[k_maxNumberOfBlocksInNode];
  size_t numberOfNodeBlocks = nodeCreator(blocks, NumberOfChildren);
  Tree<NumberOfChildren + numberOfNodeBlocks> result(blocks);

  Block * childrenAddress = &result[numberOfNodeBlocks];
  for (Node node : {static_cast<Node>(nodes)...}) {
    node.copyTreeTo(childrenAddress);
    childrenAddress += node.treeSize();
  }
  return result;
}

template<unsigned ...Len> static constexpr auto Addition(const Tree<Len> (&...children)) { return MakeTree(AdditionInterface::CreateNodeAtAddress, children...); }
template<unsigned L1, unsigned L2> static constexpr Tree<L1+L2+1> Division(const Tree<L1> child1, const Tree<L2> child2) { return MakeTree(DivisionInterface::CreateNodeAtAddress, child1, child2); }
  template<unsigned ...Len> static constexpr auto Multiplication(const Tree<Len> (&...children)) { return MakeTree(MultiplicationInterface::CreateNodeAtAddress, children...); }
  template<unsigned L1, unsigned L2> static constexpr Tree<L1+L2+1> Power(const Tree<L1> child1, const Tree<L2> child2) { return MakeTree(PowerInterface::CreateNodeAtAddress, child1, child2); }
template<unsigned L1, unsigned L2> static constexpr Tree<L1+L2+1> Subtraction(const Tree<L1> child1, const Tree<L2> child2) { return MakeTree(SubtractionInterface::CreateNodeAtAddress, child1, child2); }

static constexpr Tree<ConstantInterface::k_numberOfBlocksInNode> operator "" _n(char16_t name) {
  Tree<ConstantInterface::k_numberOfBlocksInNode> result;
  ConstantInterface::CreateNodeAtAddress(static_cast<Node>(result).block(), name);
  return result;
}

static constexpr Tree<IntegerInterface::k_minimalNumberOfBlocksInNode + 1> operator "" _n(unsigned long long value) {
  assert(value < 256); // TODO: larger values
  Tree<IntegerInterface::k_minimalNumberOfBlocksInNode + 1> result;
  IntegerInterface::CreateNodeAtAddress(static_cast<Node>(result).block(), value);
  return result;
}

}

#endif
