#include "addition_interface.h"
#include "multiplication_interface.h"
#include "../node.h"
#include "subtraction_interface.h"

namespace Poincare {

void SubtractionExpressionInterface::basicReduction(TypeBlock * block) const {
  assert(block->type() == BlockType::Division);
  return projectionReduction(block,
      []() { return AdditionInterface::PushNode(2); },
      []() { return MultiplicationInterface::PushNode(2); }
    );
}

float SubtractionExpressionInterface::approximate(const TypeBlock * treeBlock) const {
  float childrenApproximation[SubtractionInterface::k_numberOfChildren];
  for (NodeIterator::IndexedNode indexedNode : NodeIterator(Node(treeBlock)).directChildren()) {
    childrenApproximation[indexedNode.m_index] = indexedNode.m_node.expressionInterface()->approximate(indexedNode.m_node.block());
  }
  return childrenApproximation[0] - childrenApproximation[1];
}

}
