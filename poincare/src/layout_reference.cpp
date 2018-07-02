#include <poincare/layout_reference.h>
#include <poincare/layout_cursor.h>
#include <poincare/allocation_failed_layout_node.h>
#include <poincare/layout_node.h>
#include <poincare/char_layout_node.h>

namespace Poincare {

template<>
TreeNode * LayoutRef::FailedAllocationStaticNode() {
  static AllocationFailedLayoutNode FailureNode;
  if (FailureNode.identifier() >= -1) {
    int newIdentifier = TreePool::sharedPool()->registerStaticNode(&FailureNode);
    FailureNode.rename(newIdentifier);
  }
  return &FailureNode;
}

template <typename T>
LayoutCursor LayoutReference<T>::cursor() const {
  return LayoutCursor(this->typedNode());
}

template LayoutCursor LayoutReference<LayoutNode>::cursor() const;
template LayoutCursor LayoutReference<CharLayoutNode>::cursor() const;

}
