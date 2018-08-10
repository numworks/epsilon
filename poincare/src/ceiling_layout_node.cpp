#include <poincare/ceiling_layout_node.h>
#include <poincare/allocation_failure_layout_node.h>

namespace Poincare {

CeilingLayoutNode * CeilingLayoutNode::FailedAllocationStaticNode() {
  static AllocationFailureLayoutNode<CeilingLayoutNode> failure;
  TreePool::sharedPool()->registerStaticNodeIfRequired(&failure);
  return &failure;
}

}
