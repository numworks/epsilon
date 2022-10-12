#include "addition.h"
#include "../edition_reference.h"
#include "multiplication.h"
#include "../node_iterator.h"
#include "subtraction.h"

namespace Poincare {

void SubtractionExpressionInterface::basicReduction(TypeBlock * block) const {
  assert(block->type() == BlockType::Subtraction);
  return projectionReduction(block,
      []() { return AdditionInterface::PushNode(2); },
      []() { return MultiplicationInterface::PushNode(2); }
    );
}

float SubtractionExpressionInterface::approximate(const TypeBlock * treeBlock) const {
  float childrenApproximation[SubtractionInterface::k_numberOfChildren];
  for (const NodeIterator::IndexedNode indexedNode : NodeIterator(Node(treeBlock)).forwardConstChildren()) {
    childrenApproximation[indexedNode.m_index] = indexedNode.m_node.expressionInterface()->approximate(indexedNode.m_node.block());
  }
  return childrenApproximation[0] - childrenApproximation[1];
}

}
