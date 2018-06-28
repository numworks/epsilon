#include "layout_reference.h"
#include "layout_cursor.h"
#include "allocation_failed_layout_node.h"
#include "layout_node.h"
#include "char_layout_node.h"

template<>
TreeNode * LayoutRef::failedAllocationNode() {
  static AllocationFailedLayoutRef FailureRef;
  return FailureRef.node();
}

template <typename T>
LayoutCursor LayoutReference<T>::cursor() {
  return LayoutCursor(this->castedNode());
}

template LayoutCursor LayoutReference<LayoutNode>::cursor();
template LayoutCursor LayoutReference<CharLayoutNode>::cursor();
