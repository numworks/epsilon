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
  using TreeReference::operator==;
  using TreeReference::operator!=;

  LayoutReference clone() const {
    TreeReference c = this->treeClone();
    LayoutReference cast = LayoutReference(c.node()); // TODO ?
    cast.invalidAllSizesPositionsAndBaselines();
    return cast;
  }

  LayoutNode * node() const override{ return static_cast<LayoutNode *>(SerializableReference::node()); }

  // Rendering
  void draw(KDContext * ctx, KDPoint p, KDColor expressionColor = KDColorBlack, KDColor backgroundColor = KDColorWhite) {
    return this->node()->draw(ctx, p, expressionColor, backgroundColor);
  }
  void render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) {
    return this->node()->render(ctx, p, expressionColor, backgroundColor);
  }
  //TODO: check these methods are needed here, not just in Node
  KDSize layoutSize() { return this->node()->layoutSize(); }
  //KDPoint layoutOrigin() { return this->node()->origin(); }
  KDPoint absoluteOrigin() { return this->node()->absoluteOrigin(); }
  KDCoordinate baseline() { return this->node()->baseline(); }
  void invalidAllSizesPositionsAndBaselines() { return this->node()->invalidAllSizesPositionsAndBaselines(); }

  // Layout properties
  KDPoint positionOfChild(LayoutReference child) { return this->node()->positionOfChild(child.node()); }
  bool mustHaveLeftSibling() const { return const_cast<LayoutReference *>(this)->node()->mustHaveLeftSibling(); }
  bool isEmpty() const { return const_cast<LayoutReference *>(this)->node()->isEmpty(); }
  bool isHorizontal() const { return const_cast<LayoutReference *>(this)->node()->isHorizontal(); }
  bool isMatrix() const { return const_cast<LayoutReference *>(this)->node()->isMatrix(); }
  bool isVerticalOffset() const { return const_cast<LayoutReference *>(this)->node()->isVerticalOffset(); }
  bool isLeftParenthesis() const { return const_cast<LayoutReference *>(this)->node()->isLeftParenthesis(); }
  bool isCollapsable(int * numberOfOpenParenthesis, bool goingLeft) const { return const_cast<LayoutReference *>(this)->node()->isCollapsable(numberOfOpenParenthesis, goingLeft); }
  int leftCollapsingAbsorbingChildIndex() const { return const_cast<LayoutReference *>(this)->node()->leftCollapsingAbsorbingChildIndex(); }
  int rightCollapsingAbsorbingChildIndex() const { return const_cast<LayoutReference *>(this)->node()->rightCollapsingAbsorbingChildIndex(); }
  bool hasText() { return this->node()->hasText(); }
  char XNTChar() const { return const_cast<LayoutReference *>(this)->node()->XNTChar(); }

  // Layout modification
  void deleteBeforeCursor(LayoutCursor * cursor) { return this->node()->deleteBeforeCursor(cursor); }
  bool removeGreySquaresFromAllMatrixAncestors() { return this->node()->removeGreySquaresFromAllMatrixAncestors(); }
  bool addGreySquaresToAllMatrixAncestors() { return this->node()->addGreySquaresToAllMatrixAncestors(); }
  LayoutReference layoutToPointWhenInserting() { return LayoutReference(this->node()->layoutToPointWhenInserting()); }

  // Cursor
  LayoutCursor cursor() const;
  LayoutCursor equivalentCursor(LayoutCursor * cursor);

  // Tree
  LayoutReference childAtIndex(int i) {
    TreeReference treeRefChild = TreeReference::treeChildAtIndex(i);
    return LayoutReference(treeRefChild.node());
  }
  //int indexInParent() const { return this->node()->indexInParent(); }
  LayoutReference root() { return LayoutReference(this->node()->root()); }
  LayoutReference parent() { return LayoutReference(this->node()->parent()); }

  // Tree modification
  //Add
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
  // Collapse
  void collapseSiblings(LayoutCursor * cursor);
protected:
  // Add
  virtual void addChildAtIndex(LayoutReference l, int index, int currentNumberOfChildren, LayoutCursor * cursor);
  // Remove
  virtual void removeChild(LayoutReference l, LayoutCursor * cursor, bool force = false);
  virtual void removeChildAtIndex(int index, LayoutCursor * cursor, bool force = false) {
    removeChild(childAtIndex(index), cursor, force);
  }
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
