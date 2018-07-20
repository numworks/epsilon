#ifndef POINCARE_LAYOUT_REFERENCE_H
#define POINCARE_LAYOUT_REFERENCE_H

#include <poincare/layout_node.h>
#include <poincare/serializable_reference.h>

namespace Poincare {

class LayoutCursor;

template <typename T>
class LayoutReference : public SerializableReference<T> {
  friend class LayoutCursor;
public:
  using SerializableReference<T>::SerializableReference;

  LayoutReference<LayoutNode> clone() const {
    TreeReference<T> c = this->treeClone();
    LayoutReference<LayoutNode> * cast = static_cast<LayoutReference<LayoutNode> *>(&c);
    cast->invalidAllSizesPositionsAndBaselines();
    return *cast;
  }

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
  KDPoint positionOfChild(LayoutReference<LayoutNode> child) { return this->typedNode()->positionOfChild(child.typedNode()); }
  bool mustHaveLeftSibling() const { return this->typedNode()->mustHaveLeftSibling(); }
  bool isEmpty() const { return this->typedNode()->isEmpty(); }
  bool isHorizontal() const { return this->typedNode()->isHorizontal(); }
  bool isVerticalOffset() const { return this->typedNode()->isVerticalOffset(); }
  bool isLeftParenthesis() const { return this->typedNode()->isLeftParenthesis(); }
  bool isCollapsable(int * numberOfOpenParenthesis, bool goingLeft) const { return this->typedNode()->isCollapsable(numberOfOpenParenthesis, goingLeft); }
  int leftCollapsingAbsorbingChildIndex() const { return this->typedNode()->leftCollapsingAbsorbingChildIndex(); }
  int rightCollapsingAbsorbingChildIndex() const { return this->typedNode()->rightCollapsingAbsorbingChildIndex(); }
  bool hasText() { return this->typedNode()->hasText(); }
  char XNTChar() const { return this->typedNode()->XNTChar(); }

  // Layout modification
  void deleteBeforeCursor(LayoutCursor * cursor) { return this->typedNode()->deleteBeforeCursor(cursor); }
  bool removeGreySquaresFromAllMatrixAncestors() { return this->typedNode()->removeGreySquaresFromAllMatrixAncestors(); }
  bool addGreySquaresToAllMatrixAncestors() { return this->typedNode()->addGreySquaresToAllMatrixAncestors(); }
  LayoutReference<LayoutNode> layoutToPointWhenInserting() { return LayoutReference<LayoutNode>(this->typedNode()->layoutToPointWhenInserting()); }

  // Cursor
  LayoutCursor cursor() const;
  LayoutCursor equivalentCursor(LayoutCursor * cursor);

  // Tree
  LayoutReference<LayoutNode> childAtIndex(int i) {
    TreeReference<T> treeRefChild = TreeReference<T>::treeChildAtIndex(i);
    return LayoutReference<LayoutNode>(treeRefChild.node());
  }
  int indexInParent() const { return this->typedNode()->indexInParent(); }
  LayoutReference<LayoutNode> root() { return LayoutReference<LayoutNode>(this->typedNode()->root()); }
  LayoutReference<LayoutNode> parent() { return LayoutReference<LayoutNode>(this->typedNode()->parent()); }

  // Tree modification
  // Add
  void addChildAtIndex(LayoutReference<LayoutNode> l, int index, LayoutCursor * cursor);
  void addSibling(LayoutCursor * cursor, LayoutReference<LayoutNode> sibling, bool moveCursor);
  // Replace
  void replaceChildAtIndex(int oldChildIndex, LayoutReference<LayoutNode> newChild) { TreeReference<T>::replaceChildAtIndex(oldChildIndex, newChild); }
  void replaceChild(LayoutReference<LayoutNode> oldChild, LayoutReference<LayoutNode> newChild, LayoutCursor * cursor = nullptr, bool force = false);
  void replaceChildWithEmpty(LayoutReference<LayoutNode> oldChild, LayoutCursor * cursor = nullptr);
  void replaceWith(LayoutReference<LayoutNode> newChild, LayoutCursor * cursor) {
    LayoutReference<LayoutNode> p = parent();
    assert(p.isDefined());
    p.replaceChild(*this, newChild, cursor);
  }
  void replaceWithJuxtapositionOf(LayoutReference<LayoutNode> leftChild, LayoutReference<LayoutNode> rightChild, LayoutCursor * cursor, bool putCursorInTheMiddle = false);
  // Remove
  void removeChild(LayoutReference<LayoutNode> l, LayoutCursor * cursor, bool force = false);
  void removeChildAtIndex(int index, LayoutCursor * cursor, bool force = false) {
    return removeChild(childAtIndex(index), cursor, force);
  }
  // Collapse
  void collapseSiblings(LayoutCursor * cursor);

private:
  // Tree modification
  enum class HorizontalDirection {
    Left,
    Right
  };
  void collapseOnDirection(HorizontalDirection direction, int absorbingChildIndex);
};

typedef LayoutReference<LayoutNode> LayoutRef;

}

#endif
