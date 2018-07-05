#include <poincare/layout_reference.h>
#include <poincare/layout_cursor.h>
#include <poincare/allocation_failed_layout_node.h>
#include <poincare/layout_node.h>
#include <poincare/layout_cursor.h>
#include <poincare/char_layout_node.h>
#include <poincare/horizontal_layout_node.h>

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

// Cursor
template <typename T>
LayoutCursor LayoutReference<T>::cursor() const {
  return LayoutCursor(this->typedNode());
}

template<>
LayoutCursor LayoutRef::equivalentCursor(LayoutCursor * cursor) {
  return this->typedNode()->equivalentCursor(cursor);
}

// Tree modification
template<>
LayoutReference<LayoutNode> LayoutRef::replaceWithJuxtapositionOf(LayoutReference<LayoutNode> leftChild, LayoutReference<LayoutNode> rightChild) {
  LayoutReference<LayoutNode> p = parent();
  assert(p.isDefined());
  assert(!p.isHorizontal());
  /* One of the children to juxtapose might be "this", so we cannot just call
   * replaceWith. */
  LayoutReference<LayoutNode> horizontalLayoutR = HorizontalLayoutRef();
  int index = indexInParent();
  horizontalLayoutR.addChildAtIndex(leftChild, 0);
  horizontalLayoutR.addChildAtIndex(rightChild, 1);
  p.addChildAtIndex(horizontalLayoutR, index);
  return horizontalLayoutR;
}

template LayoutCursor LayoutReference<LayoutNode>::cursor() const;
template LayoutCursor LayoutReference<CharLayoutNode>::cursor() const;

}
