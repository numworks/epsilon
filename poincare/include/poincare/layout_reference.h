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
  LayoutReference& operator=(LayoutReference<LayoutNode>& lr) {
    this->setTo(lr);
    return *this;
  }
  LayoutReference& operator=(const LayoutReference<LayoutNode>& lr) {
    this->setTo(lr);
    return *this;
  }
  inline bool operator==(LayoutReference<LayoutNode> l) { return this->identifier() == l.identifier(); }
  inline bool operator!=(LayoutReference<LayoutNode> l) { return this->identifier() != l.identifier(); }

  static TreeNode * FailedAllocationStaticNode();

  LayoutCursor cursor() const;

  LayoutReference<LayoutNode> childAtIndex(int i) {
    TreeReference<T> treeRefChild = TreeReference<T>::treeChildAtIndex(i);
    return LayoutReference<LayoutNode>(treeRefChild.node());
  }

  void replaceChildAtIndex(int oldChildIndex, LayoutReference<LayoutNode> newChild) {
    TreeReference<T>::replaceChildAtIndex(oldChildIndex, newChild);
  }

  bool isHorizontal() const { return this->typedNode()->isHorizontal(); }
  bool hasText() { return this->typedNode()->hasText(); }
  char XNTChar() const { return this->typedNode()->XNTChar(); }
  KDSize layoutSize() { return this->typedNode()->layoutSize(); }
  KDPoint layoutOrigin() { return this->typedNode()->layoutOrigin(); }
  KDPoint absoluteOrigin() { return this->typedNode()->absoluteOrigin(); }
  KDCoordinate baseline() { return this->typedNode()->baseline(); }
  LayoutCursor equivalentCursor(LayoutCursor * cursor);
  void invalidAllSizesPositionsAndBaselines() { return this->typedNode()->invalidAllSizesPositionsAndBaselines(); }
  bool removeGreySquaresFromAllMatrixAncestors() { return this->typedNode()->removeGreySquaresFromAllMatrixAncestors(); }
  bool addGreySquaresToAllMatrixAncestors() { return this->typedNode()->addGreySquaresToAllMatrixAncestors(); }
  LayoutReference<LayoutNode> layoutToPointWhenInserting() { return LayoutReference<LayoutNode>(this->typedNode()->layoutToPointWhenInserting()); }
};

typedef LayoutReference<LayoutNode> LayoutRef;

}

#endif
