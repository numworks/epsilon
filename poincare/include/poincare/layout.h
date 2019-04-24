#ifndef POINCARE_LAYOUT_REFERENCE_H
#define POINCARE_LAYOUT_REFERENCE_H

#include <poincare/array_builder.h>
#include <poincare/layout_node.h>
#include <poincare/tree_handle.h>

namespace Poincare {

class LayoutCursor;

class Layout : public TreeHandle {
  friend class GridLayoutNode;
  friend class HorizontalLayoutNode;
  friend class LayoutNode;
  friend class LayoutCursor;
  friend class VerticalOffsetLayoutNode;
public:
  Layout() : TreeHandle() {}
  Layout(const LayoutNode * node) : TreeHandle(node) {}
  Layout clone() const;
  LayoutNode * node() const {
    assert(isUninitialized() || !TreeHandle::node()->isGhost());
    return static_cast<LayoutNode *>(TreeHandle::node());
  }

  // Properties
  LayoutNode::Type type() const { return node()->type(); }

  // Rendering
  void draw(KDContext * ctx, KDPoint p, KDColor expressionColor = KDColorBlack, KDColor backgroundColor = KDColorWhite) {
    return node()->draw(ctx, p, expressionColor, backgroundColor);
  }
  void render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) {
    return node()->render(ctx, p, expressionColor, backgroundColor);
  }
  KDSize layoutSize() { return node()->layoutSize(); }
  KDPoint absoluteOrigin() { return node()->absoluteOrigin(); }
  KDCoordinate baseline() { return node()->baseline(); }
  void invalidAllSizesPositionsAndBaselines() { return node()->invalidAllSizesPositionsAndBaselines(); }

  // Serialization
  int serializeForParsing(char * buffer, int bufferSize) const { return node()->serialize(buffer, bufferSize); }
  int serializeParsedExpression(char * buffer, int bufferSize) const;

  // Layout properties
  typedef bool (*LayoutTest)(const Layout l);
  Layout recursivelyMatches(LayoutTest test) const;
  bool mustHaveLeftSibling() const { return const_cast<Layout *>(this)->node()->mustHaveLeftSibling(); }
  bool isEmpty() const { return const_cast<Layout *>(this)->node()->isEmpty(); }
  bool isCollapsable(int * numberOfOpenParenthesis, bool goingLeft) const { return const_cast<Layout *>(this)->node()->isCollapsable(numberOfOpenParenthesis, goingLeft); }
  int leftCollapsingAbsorbingChildIndex() const { return const_cast<Layout *>(this)->node()->leftCollapsingAbsorbingChildIndex(); }
  int rightCollapsingAbsorbingChildIndex() const { return const_cast<Layout *>(this)->node()->rightCollapsingAbsorbingChildIndex(); }
  bool hasText() { return node()->hasText(); }
  CodePoint XNTCodePoint() const { return const_cast<Layout *>(this)->node()->XNTCodePoint(); }

  // Layout modification
  void deleteBeforeCursor(LayoutCursor * cursor) { return node()->deleteBeforeCursor(cursor); }
  bool removeGreySquaresFromAllMatrixAncestors() { return node()->removeGreySquaresFromAllMatrixAncestors(); }
  bool addGreySquaresToAllMatrixAncestors() { return node()->addGreySquaresToAllMatrixAncestors(); }
  Layout layoutToPointWhenInserting() { return Layout(node()->layoutToPointWhenInserting()); }

  // Cursor
  LayoutCursor cursor() const;
  LayoutCursor equivalentCursor(LayoutCursor * cursor);

  // Tree
  Layout childAtIndex(int i) const;
  Layout root() const {
    assert(!isUninitialized());
    return Layout(node()->root());
  }
  Layout parent() const {
    assert(!isUninitialized());
    return Layout(node()->parent());
  }

  // Tree modification
  //Add
  void addSibling(LayoutCursor * cursor, Layout sibling, bool moveCursor);
  // Replace
  void replaceChild(Layout oldChild, Layout newChild, LayoutCursor * cursor = nullptr, bool force = false);
  void replaceChildWithEmpty(Layout oldChild, LayoutCursor * cursor = nullptr);
  void replaceWith(Layout newChild, LayoutCursor * cursor);
  void replaceWithJuxtapositionOf(Layout leftChild, Layout rightChild, LayoutCursor * cursor, bool putCursorInTheMiddle = false);
  // Collapse
  void collapseSiblings(LayoutCursor * cursor);
protected:
  // Add
  void addChildAtIndex(Layout l, int index, int currentNumberOfChildren, LayoutCursor * cursor);
  // Remove
  void removeChild(Layout l, LayoutCursor * cursor, bool force = false);
  void removeChildAtIndex(int index, LayoutCursor * cursor, bool force = false);
private:
  // Tree modification
  enum class HorizontalDirection {
    Left,
    Right
  };
  void collapseOnDirection(HorizontalDirection direction, int absorbingChildIndex);
};

}

#endif
