#include "multiplication_interface.h"
#include "../node.h"

namespace Poincare {

float MultiplicationExpressionInterface::approximate(const TypeBlock * block) const {
  float res = 1.0f;
  for (NodeIterator::IndexedNode indexedNode : NodeIterator(Node(block)).directChildren()) {
    res *= indexedNode.m_node.expressionInterface()->approximate(indexedNode.m_node.block());
  }
  return res;
}

}
