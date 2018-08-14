#include "pair_node.h"

namespace Poincare {

PairNode * PairNode::FailedAllocationStaticNode() {
  static AllocationFailurePairNode failureNode;
  TreePool::sharedPool()->registerStaticNodeIfRequired(&failureNode);
  return &failureNode;
}

}
