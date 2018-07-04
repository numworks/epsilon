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

  // Operators

  // Allow every LayoutReference<T> to be transformed into a LayoutRef
  operator LayoutReference<LayoutNode>() const { return LayoutReference<LayoutNode>(this->node()); }
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

  // Rendering
  void draw(KDContext * ctx, KDPoint p, KDColor expressionColor = KDColorBlack, KDColor backgroundColor = KDColorWhite) {
    return this->typedNode()->draw(ctx, p, expressionColor, backgroundColor);
  }
  KDSize layoutSize() { return this->typedNode()->layoutSize(); }
  KDPoint layoutOrigin() { return this->typedNode()->layoutOrigin(); }
  KDPoint absoluteOrigin() { return this->typedNode()->absoluteOrigin(); }
  KDCoordinate baseline() { return this->typedNode()->baseline(); }
  void invalidAllSizesPositionsAndBaselines() { return this->typedNode()->invalidAllSizesPositionsAndBaselines(); }

  // Layout properties
  bool isHorizontal() const { return this->typedNode()->isHorizontal(); }
  bool isLeftParenthesis() const { return this->typedNode()->isLeftParenthesis(); }
  bool hasText() { return this->typedNode()->hasText(); }
  char XNTChar() const { return this->typedNode()->XNTChar(); }

  // Layout modification
  bool removeGreySquaresFromAllMatrixAncestors() { return this->typedNode()->removeGreySquaresFromAllMatrixAncestors(); }
  bool addGreySquaresToAllMatrixAncestors() { return this->typedNode()->addGreySquaresToAllMatrixAncestors(); }
  LayoutReference<LayoutNode> layoutToPointWhenInserting() { return LayoutReference<LayoutNode>(this->typedNode()->layoutToPointWhenInserting()); }

  // Cursor
  LayoutCursor cursor() const;
  LayoutCursor equivalentCursor(LayoutCursor * cursor);

  // Hierarchy

  LayoutReference<LayoutNode> childAtIndex(int i) {
    TreeReference<T> treeRefChild = TreeReference<T>::treeChildAtIndex(i);
    return LayoutReference<LayoutNode>(treeRefChild.node());
  }
  LayoutReference<LayoutNode> root() { return LayoutReference<LayoutNode>(this->typedNode()->root()); }

  // Hierarchy modification
  void replaceChildAtIndex(int oldChildIndex, LayoutReference<LayoutNode> newChild) {
    TreeReference<T>::replaceChildAtIndex(oldChildIndex, newChild);
  }
  void addSiblingAndMoveCursor(LayoutCursor * cursor, LayoutReference<LayoutNode> sibling) { return this->typedNode()->addSiblingAndMoveCursor(cursor, sibling.typedNode()); } //TODO
  void collapseSiblingsAndMoveCursor(LayoutCursor * cursor) {} //TODO
  LayoutReference<LayoutNode> replaceWithJuxtapositionOf(LayoutReference<LayoutNode> leftChild, LayoutReference<LayoutNode> rightChild); //TODO

  // Allocation failure
  static TreeNode * FailedAllocationStaticNode();
};

typedef LayoutReference<LayoutNode> LayoutRef;

}

#endif
