#ifndef POINCARE_LAYOUT_REFERENCE_H
#define POINCARE_LAYOUT_REFERENCE_H

#include <poincare/layout_node.h>
#include <poincare/serializable_reference.h>

namespace Poincare {

class LayoutCursor;

class LayoutReference : public SerializableReference {
  friend class LayoutCursor;
public:
  using SerializableReference::SerializableReference;

  LayoutReference clone() const {
    TreeReference c = this->treeClone();
    LayoutReference cast = LayoutReference(c.node()); // TODO ?
    cast.invalidAllSizesPositionsAndBaselines();
    return cast;
  }

  // Operators
  //TODO Remove these operators? If so, put setTo private
  // Allow every LayoutReference to be transformed into a LayoutRef
  LayoutReference& operator=(LayoutReference& lr) {
    this->setTo(lr);
    return *this;
  }
  LayoutReference& operator=(const LayoutReference& lr) {
    this->setTo(lr);
    return *this;
  }
  inline bool operator==(LayoutReference l) { return this->identifier() == l.identifier(); }
  inline bool operator!=(LayoutReference l) { return this->identifier() != l.identifier(); }

  LayoutNode * typedNode() { return static_cast<LayoutNode *>(node()); }

  // Rendering
  void draw(KDContext * ctx, KDPoint p, KDColor expressionColor = KDColorBlack, KDColor backgroundColor = KDColorWhite) {
    return this->typedNode()->draw(ctx, p, expressionColor, backgroundColor);
  }
  void render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) {
    return this->typedNode()->render(ctx, p, expressionColor, backgroundColor);
  }
  //TODO: check these methods are needed here, not just in Node
  KDSize layoutSize() { return this->typedNode()->layoutSize(); }
  //KDPoint layoutOrigin() { return this->typedNode()->origin(); }
  KDPoint absoluteOrigin() { return this->typedNode()->absoluteOrigin(); }
  KDCoordinate baseline() { return this->typedNode()->baseline(); }
  void invalidAllSizesPositionsAndBaselines() { return this->typedNode()->invalidAllSizesPositionsAndBaselines(); }

  // Layout properties
  KDPoint positionOfChild(LayoutReference child) { return this->typedNode()->positionOfChild(child.typedNode()); }
  bool mustHaveLeftSibling() const { return const_cast<LayoutReference *>(this)->typedNode()->mustHaveLeftSibling(); }
  bool isEmpty() const { return const_cast<LayoutReference *>(this)->typedNode()->isEmpty(); }
  bool isHorizontal() const { return const_cast<LayoutReference *>(this)->typedNode()->isHorizontal(); }
  bool isVerticalOffset() const { return const_cast<LayoutReference *>(this)->typedNode()->isVerticalOffset(); }
  bool isLeftParenthesis() const { return const_cast<LayoutReference *>(this)->typedNode()->isLeftParenthesis(); }
  bool isCollapsable(int * numberOfOpenParenthesis, bool goingLeft) const { return const_cast<LayoutReference *>(this)->typedNode()->isCollapsable(numberOfOpenParenthesis, goingLeft); }
  int leftCollapsingAbsorbingChildIndex() const { return const_cast<LayoutReference *>(this)->typedNode()->leftCollapsingAbsorbingChildIndex(); }
  int rightCollapsingAbsorbingChildIndex() const { return const_cast<LayoutReference *>(this)->typedNode()->rightCollapsingAbsorbingChildIndex(); }
  bool hasText() { return this->typedNode()->hasText(); }
  char XNTChar() const { return const_cast<LayoutReference *>(this)->typedNode()->XNTChar(); }

  // Layout modification
  void deleteBeforeCursor(LayoutCursor * cursor) { return this->typedNode()->deleteBeforeCursor(cursor); }
  bool removeGreySquaresFromAllMatrixAncestors() { return this->typedNode()->removeGreySquaresFromAllMatrixAncestors(); }
  bool addGreySquaresToAllMatrixAncestors() { return this->typedNode()->addGreySquaresToAllMatrixAncestors(); }
  LayoutReference layoutToPointWhenInserting() { return LayoutReference(this->typedNode()->layoutToPointWhenInserting()); }

  // Cursor
  LayoutCursor cursor() const;
  LayoutCursor equivalentCursor(LayoutCursor * cursor);

  // Tree
  LayoutReference childAtIndex(int i) {
    TreeReference treeRefChild = TreeReference::treeChildAtIndex(i);
    return LayoutReference(treeRefChild.node());
  }
  //int indexInParent() const { return this->typedNode()->indexInParent(); }
  LayoutReference root() { return LayoutReference(this->typedNode()->root()); }
  LayoutReference parent() { return LayoutReference(this->typedNode()->parent()); }

  // Tree modification
  // Add
  void addChildAtIndex(LayoutReference l, int index, LayoutCursor * cursor);
  void addSibling(LayoutCursor * cursor, LayoutReference sibling, bool moveCursor);
  // Replace
  //void replaceChildAtIndex(int oldChildIndex, LayoutReference newChild) { TreeReference::replaceTreeChildAtIndex(oldChildIndex, newChild); }
  void replaceChild(LayoutReference oldChild, LayoutReference newChild, LayoutCursor * cursor = nullptr, bool force = false);
  void replaceChildWithEmpty(LayoutReference oldChild, LayoutCursor * cursor = nullptr);
  void replaceWith(LayoutReference newChild, LayoutCursor * cursor) {
    LayoutReference p = parent();
    assert(p.isDefined());
    p.replaceChild(*this, newChild, cursor);
  }
  void replaceWithJuxtapositionOf(LayoutReference leftChild, LayoutReference rightChild, LayoutCursor * cursor, bool putCursorInTheMiddle = false);
  // Remove
  void removeChild(LayoutReference l, LayoutCursor * cursor, bool force = false);
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

typedef LayoutReference LayoutRef;

}

#endif
