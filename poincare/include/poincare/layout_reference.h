#ifndef POINCARE_LAYOUT_REFERENCE_H
#define POINCARE_LAYOUT_REFERENCE_H

#include "tree_reference.h"
#include "layout_node.h"

namespace Poincare {

class LayoutCursor;

template <typename T>
class LayoutReference : public TreeReference<T> {
  friend class LayoutCursor;
public:
  using TreeReference<T>::TreeReference;

  /* Allow every LayoutReference<T> to be transformed into a
   * LayoutReference<LayoutNode>, i.e. Layout */
  operator LayoutReference<LayoutNode>() const {
    return LayoutReference<LayoutNode>(this->node());
  }

  static TreeNode * FailedAllocationStaticNode();

  LayoutCursor cursor() const;

  LayoutReference<LayoutNode> childAtIndex(int i) {
    TreeReference<T> treeRefChild = TreeReference<T>::treeChildAtIndex(i);
    return LayoutReference<LayoutNode>(treeRefChild.node());
  }

  void replaceChildAtIndex(int oldChildIndex, LayoutReference<LayoutNode> newChild) {
    TreeReference<T>::replaceChildAtIndex(oldChildIndex, newChild);
  }

  int layoutOrigin() {
    return this->typedNode()->layoutOrigin();
  }

  int absoluteOrigin() {
    return this->typedNode()->absoluteOrigin();
  }
};

typedef LayoutReference<LayoutNode> LayoutRef;

}

#endif
