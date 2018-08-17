#include <poincare/ghost_node.h>
#include <poincare/tree_pool.h>

namespace Poincare {

GhostNode * GhostNode::FailedAllocationStaticNode() {
  static AllocationFailedGhostNode failure;
  TreePool::sharedPool()->registerStaticNodeIfRequired(&failure);
  return &failure;
}

}
