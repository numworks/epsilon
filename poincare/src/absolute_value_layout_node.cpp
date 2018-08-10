#include <poincare/absolute_value_layout_node.h>

namespace Poincare {

AbsoluteValueLayoutNode * AbsoluteValueLayoutNode::FailedAllocationStaticNode() {
  static AllocationFailureLayoutNode<AbsoluteValueLayoutNode> failure;
  TreePool::sharedPool()->registerStaticNodeIfRequired(&failure);
  return &failure;
}

}
