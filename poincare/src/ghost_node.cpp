#include <poincare/ghost_node.h>
#include <poincare/tree_pool.h>

namespace Poincare {

TreeNode * GhostNode::uninitializedStaticNode() const {
  return UninitializedGhostNode::UninitializedGhostStaticNode();
}

GhostNode * GhostNode::FailedAllocationStaticNode() {
  static AllocationFailedGhostNode failure;
  TreePool::sharedPool()->registerStaticNodeIfRequired(&failure);
  return &failure;
}

UninitializedGhostNode * UninitializedGhostNode::UninitializedGhostStaticNode() {
  static UninitializedGhostNode exception;
  TreePool::sharedPool()->registerStaticNodeIfRequired(&exception);
  return &exception;
}

}
