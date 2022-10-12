#include "constant.h"
#include "multiplication.h"
#include "power.h"
#include "../edition_reference.h"
#include "../node_iterator.h"

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
  for (const NodeIterator::IndexedNode indexedNode : NodeIterator(Node(treeBlock)).forwardConstChildren()) {
    childrenApproximation[indexedNode.m_index] = indexedNode.m_node.expressionInterface()->approximate(indexedNode.m_node.block());
  }
  return childrenApproximation[0] / childrenApproximation[1];
}

}
