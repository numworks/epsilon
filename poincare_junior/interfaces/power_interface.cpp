#include <cmath>
#include "../node.h"
#include "power_interface.h"

namespace Poincare {

float PowerExpressionInterface::approximate(const TypeBlock * block) const {
  float childrenApproximation[DivisionInterface::k_numberOfChildren];
  for (NodeIterator::IndexedNode indexedNode : NodeIterator(Node(block)).directChildren()) {
    childrenApproximation[indexedNode.m_index] = indexedNode.m_node.expressionInterface()->approximate(indexedNode.m_node.block());
  }
  return std::pow(childrenApproximation[0], childrenApproximation[1]);
}


}
