#include <poincare/ghost_node.h>

namespace Poincare {

GhostNode * GhostNode::FailedAllocationStaticNode() {
  static AllocationFailedGhostNode FailureNode;
  TreePool::sharedPool()->registerStaticNodeIfRequired(&FailureNode);
  return &FailureNode;
}

}
