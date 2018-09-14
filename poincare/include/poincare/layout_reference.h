#ifndef POINCARE_LAYOUT_REFERENCE_H
#define POINCARE_LAYOUT_REFERENCE_H

#include <poincare/layout_node.h>
#include <poincare/tree_by_reference.h>

namespace Poincare {

class LayoutCursor;

class LayoutReference : public TreeByReference {
  friend class GridLayoutNode;
  friend class HorizontalLayoutNode;
  friend class LayoutNode;
  friend class LayoutCursor;
  friend class VerticalOffsetLayoutNode;
public:
  LayoutReference() : TreeByReference() {}
  LayoutReference(const LayoutNode * node) : TreeByReference(node) {}
  LayoutReference clone() const;
  LayoutNode * node() const {
    assert(isUninitialized() || !TreeByReference::node()->isGhost());
    return static_cast<LayoutNode *>(TreeByReference::node());
  }

  // Rendering
  void draw(KDContext * ctx, KDPoint p, KDColor expressionColor = KDColorBlack, KDColor backgroundColor = KDColorWhite) {
    return node()->draw(ctx, p, expressionColor, backgroundColor);
  }
  void render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) {
    return node()->render(ctx, p, expressionColor, backgroundColor);
  }
  //TODO: check these methods are needed here, not just in Node
  KDSize layoutSize() { return node()->layoutSize(); }
  KDPoint absoluteOrigin() { return node()->absoluteOrigin(); }
  KDCoordinate baseline() { return node()->baseline(); }
  void invalidAllSizesPositionsAndBaselines() { return node()->invalidAllSizesPositionsAndBaselines(); }

  // Serialization
  int serialize(char * buffer, int bufferSize) const { return node()->serialize(buffer, bufferSize); }

  // Layout properties
  KDPoint positionOfChild(LayoutReference child) { return node()->positionOfChild(child.node()); }
  bool mustHaveLeftSibling() const { return const_cast<LayoutReference *>(this)->node()->mustHaveLeftSibling(); }
  bool isEmpty() const { return const_cast<LayoutReference *>(this)->node()->isEmpty(); }
  bool isHorizontal() const { return const_cast<LayoutReference *>(this)->node()->isHorizontal(); }
  bool isMatrix() const { return const_cast<LayoutReference *>(this)->node()->isMatrix(); }
  bool isVerticalOffset() const { return const_cast<LayoutReference *>(this)->node()->isVerticalOffset(); }
  bool isLeftParenthesis() const { return const_cast<LayoutReference *>(this)->node()->isLeftParenthesis(); }
  bool isCollapsable(int * numberOfOpenParenthesis, bool goingLeft) const { return const_cast<LayoutReference *>(this)->node()->isCollapsable(numberOfOpenParenthesis, goingLeft); }
  int leftCollapsingAbsorbingChildIndex() const { return const_cast<LayoutReference *>(this)->node()->leftCollapsingAbsorbingChildIndex(); }
  int rightCollapsingAbsorbingChildIndex() const { return const_cast<LayoutReference *>(this)->node()->rightCollapsingAbsorbingChildIndex(); }
  bool hasText() { return node()->hasText(); }
  char XNTChar() const { return const_cast<LayoutReference *>(this)->node()->XNTChar(); }

  // Layout modification
  void deleteBeforeCursor(LayoutCursor * cursor) { return node()->deleteBeforeCursor(cursor); }
  bool removeGreySquaresFromAllMatrixAncestors() { return node()->removeGreySquaresFromAllMatrixAncestors(); }
  bool addGreySquaresToAllMatrixAncestors() { return node()->addGreySquaresToAllMatrixAncestors(); }
  LayoutReference layoutToPointWhenInserting() { return LayoutReference(node()->layoutToPointWhenInserting()); }

  // Cursor
  LayoutCursor cursor() const;
  LayoutCursor equivalentCursor(LayoutCursor * cursor);

  // Tree
  LayoutReference childAtIndex(int i);
  LayoutReference root() const {
    assert(!isUninitialized());
    return LayoutReference(node()->root());
  }
  LayoutReference parent() const {
    assert(!isUninitialized());
    return LayoutReference(node()->parent());
  }

  // Tree modification
  //Add
  void addSibling(LayoutCursor * cursor, LayoutReference sibling, bool moveCursor);
  // Replace
  void replaceChild(LayoutReference oldChild, LayoutReference newChild, LayoutCursor * cursor = nullptr, bool force = false);
  void replaceChildWithEmpty(LayoutReference oldChild, LayoutCursor * cursor = nullptr);
  void replaceWith(LayoutReference newChild, LayoutCursor * cursor);
  void replaceWithJuxtapositionOf(LayoutReference leftChild, LayoutReference rightChild, LayoutCursor * cursor, bool putCursorInTheMiddle = false);
  // Collapse
  void collapseSiblings(LayoutCursor * cursor);
protected:
  // Add
  void addChildAtIndex(LayoutReference l, int index, int currentNumberOfChildren, LayoutCursor * cursor);
  // Remove
  void removeChild(LayoutReference l, LayoutCursor * cursor, bool force = false);
  void removeChildAtIndex(int index, LayoutCursor * cursor, bool force = false);
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
