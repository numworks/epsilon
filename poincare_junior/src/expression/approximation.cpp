#include "approximation.h"
#include "expression.h"
#include "../node_iterator.h"

namespace Poincare {

float Approximation::MapAndReduce(const TypeBlock * block, Reductor reductor) {
  float res;
  for (const NodeIterator::IndexedNode indexedNode : NodeIterator(Node(block)).forwardConstChildren()) {
    float app = Expression::Approximate(indexedNode.m_node.block());
    if (indexedNode.m_index == 0) {
      res = app;
    } else {
      res = reductor(res, app);
    }
  }
  return res;
}

}
