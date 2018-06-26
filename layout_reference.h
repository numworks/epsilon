#ifndef LAYOUT_REFERENCE_H
#define LAYOUT_REFERENCE_H

#include "tree_reference.h"
#include "layout_node.h"

class LayoutCursor;

template <typename T>
class LayoutReference : public TreeReference<T> {
  friend class LayoutCursor;
public:
  using TreeReference<T>::TreeReference;

  /* Allow every LayoutReference<T> to be transformed into a
   * LayoutReference<LayoutNode>, i.e. Layout */
  operator LayoutReference<LayoutNode>() const {
    // TODO: make sure this is kosher
    return *(reinterpret_cast<const LayoutReference<LayoutNode> *>(this));
  }

  LayoutReference<LayoutNode> * pointer() {
    return (reinterpret_cast<LayoutReference<LayoutNode> *>(this));
  }

  LayoutCursor cursor() const;

  virtual void addChild(LayoutReference<LayoutNode> l) {
    TreeReference<T>::addChild(l);
  }

  LayoutReference<LayoutNode> childAtIndex(int i) {
    TreeReference<T> treeRefChild = TreeReference<T>::treeChildAtIndex(i);
    return LayoutReference<LayoutNode>(treeRefChild.node());
  }

  void replaceChildAtIndex(int oldChildIndex, LayoutReference<LayoutNode> newChild) {
    TreeReference<T>::replaceChildAtIndex(oldChildIndex, newChild);
  }

  int layoutOrigin() {
    return this->node()->layoutOrigin();
  }

  int absoluteOrigin() {
    return this->node()->absoluteOrigin();
  }
};

typedef LayoutReference<LayoutNode> LayoutRef;

#endif
