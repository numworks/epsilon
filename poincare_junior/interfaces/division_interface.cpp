#include "constant_interface.h"
#include "multiplication_interface.h"
#include "power_interface.h"
#include "../node.h"

namespace Poincare {

void DivisionExpressionInterface::basicReduction(TypeBlock * block) const {
  assert(block->type() == BlockType::Division);
  return projectionReduction(block,
      []() { return MultiplicationInterface::PushNode(2); },
      PowerInterface::PushNode
    );
}

float DivisionExpressionInterface::approximate(const TypeBlock * treeBlock) const {
  float childrenApproximation[DivisionInterface::k_numberOfChildren];
  for (NodeIterator::IndexedNode indexedNode : NodeIterator(Node(treeBlock)).directChildren()) {
    childrenApproximation[indexedNode.m_index] = indexedNode.m_node.expressionInterface()->approximate(indexedNode.m_node.block());
  }
  return childrenApproximation[0] / childrenApproximation[1];
}

}
