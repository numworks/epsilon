#include <poincare/ghost_node.h>
#include <poincare/uninitialized_ghost_node.h>
#include <poincare/tree_pool.h>

namespace Poincare {

GhostNode * GhostNode::FailedAllocationStaticNode() {
  static AllocationFailedGhostNode failure;
  TreePool::sharedPool()->registerStaticNodeIfRequired(&failure);
  return &failure;
}

TreeNode * GhostNode::uninitializedStaticNode() const {
  return UninitializedGhostNode::UninitializedGhostStaticNode();
}

}
