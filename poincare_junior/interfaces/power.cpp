#include <cmath>
#include "../edition_reference.h"
#include "power.h"

namespace Poincare {

float PowerExpressionInterface::approximate(const TypeBlock * block) const {
  float childrenApproximation[DivisionInterface::k_numberOfChildren];
  for (const Iterator::IndexedNode indexedNode : Iterator(Node(block)).forwardConstChildren()) {
    childrenApproximation[indexedNode.m_index] = indexedNode.m_node.expressionInterface()->approximate(indexedNode.m_node.block());
  }
  return std::pow(childrenApproximation[0], childrenApproximation[1]);
}


}
