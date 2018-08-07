#include <poincare/ghost_node.h>

namespace Poincare {

TreeNode * GhostNode::FailedAllocationStaticNode() {
  static AllocationFailedGhostNode FailureNode;
  TreePool::sharedPool()->registerStaticNodeIfRequired(&FailureNode);
  return &FailureNode;
}

}
