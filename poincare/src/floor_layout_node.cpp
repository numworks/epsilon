#include <poincare/floor_layout_node.h>
#include <poincare/allocation_failure_layout_node.h>

namespace Poincare {

FloorLayoutNode * FloorLayoutNode::FailedAllocationStaticNode() {
  static AllocationFailureLayoutNode<FloorLayoutNode> failure;
  TreePool::sharedPool()->registerStaticNodeIfRequired(&failure);
  return &failure;
}

}
