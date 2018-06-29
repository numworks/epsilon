#include "layout_reference.h"
#include "layout_cursor.h"
#include "allocation_failed_layout_node.h"
#include "layout_node.h"
#include "char_layout_node.h"

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
  return LayoutCursor(this->castedNode());
}

template LayoutCursor LayoutReference<LayoutNode>::cursor() const;
template LayoutCursor LayoutReference<CharLayoutNode>::cursor() const;
