#ifndef POINCARE_UNINITIALIZED_LAYOUT_NODE_H
#define POINCARE_UNINITIALIZED_LAYOUT_NODE_H

#include <poincare/exception_layout_node.h>
#include <stdio.h>

namespace Poincare {

/* All UninitializedExpressions should be caught so its node methods are
 * asserted false. */

class UninitializedLayoutNode : public ExceptionLayoutNode<LayoutNode> {
public:
  static UninitializedLayoutNode * UninitializedLayoutStaticNode();

  // LayoutNode
  // Rendering
  void invalidAllSizesPositionsAndBaselines() override { assert(false); ExceptionLayoutNode<LayoutNode>::invalidAllSizesPositionsAndBaselines(); }

  // Tree navigation
  void moveCursorLeft(LayoutCursor * cursor, bool * shouldRecomputeLayout) override { assert(false); ExceptionLayoutNode<LayoutNode>::moveCursorLeft(cursor, shouldRecomputeLayout); }
  void moveCursorRight(LayoutCursor * cursor, bool * shouldRecomputeLayout) override { assert(false); ExceptionLayoutNode<LayoutNode>::moveCursorRight(cursor, shouldRecomputeLayout); }
  void moveCursorUp(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited = false) override { assert(false); ExceptionLayoutNode<LayoutNode>::moveCursorUp(cursor, shouldRecomputeLayout, equivalentPositionVisited); }
  void moveCursorDown(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited = false) override { assert(false); ExceptionLayoutNode<LayoutNode>::moveCursorDown(cursor, shouldRecomputeLayout, equivalentPositionVisited); }
  LayoutCursor equivalentCursor(LayoutCursor * cursor) override { assert(false); return ExceptionLayoutNode<LayoutNode>::equivalentCursor(cursor); }

  // Tree modification

  // Collapse
  bool shouldCollapseSiblingsOnLeft() const override { assert(false); return ExceptionLayoutNode<LayoutNode>::shouldCollapseSiblingsOnLeft(); }
  bool shouldCollapseSiblingsOnRight() const override { assert(false); return ExceptionLayoutNode<LayoutNode>::shouldCollapseSiblingsOnRight(); }
  void didCollapseSiblings(LayoutCursor * cursor) override { assert(false); ExceptionLayoutNode<LayoutNode>::didCollapseSiblings(cursor);}

  //User input
  void deleteBeforeCursor(LayoutCursor * cursor) override { assert(false); ExceptionLayoutNode<LayoutNode>::deleteBeforeCursor(cursor);}

  // Other
  LayoutNode * layoutToPointWhenInserting() override { assert(false); return ExceptionLayoutNode<LayoutNode>::layoutToPointWhenInserting(); }
  bool hasText() const override { assert(false); return ExceptionLayoutNode<LayoutNode>::hasText(); }
  bool isCollapsable(int * numberOfOpenParenthesis, bool goingLeft) const override { assert(false); return ExceptionLayoutNode<LayoutNode>::isCollapsable(numberOfOpenParenthesis, goingLeft); }
  bool canBeOmittedMultiplicationLeftFactor() const override { assert(false); return ExceptionLayoutNode<LayoutNode>::canBeOmittedMultiplicationLeftFactor(); }
  bool canBeOmittedMultiplicationRightFactor() const override { assert(false); return ExceptionLayoutNode<LayoutNode>::canBeOmittedMultiplicationRightFactor(); }
  bool mustHaveLeftSibling() const override { assert(false); return ExceptionLayoutNode<LayoutNode>::mustHaveLeftSibling(); }
  bool isVerticalOffset() const override { assert(false); return ExceptionLayoutNode<LayoutNode>::isVerticalOffset(); }
  bool isHorizontal() const override { assert(false); return ExceptionLayoutNode<LayoutNode>::isHorizontal(); }
  bool isLeftParenthesis() const override { assert(false); return ExceptionLayoutNode<LayoutNode>::isLeftParenthesis(); }
  bool isRightParenthesis() const override { assert(false); return ExceptionLayoutNode<LayoutNode>::isRightParenthesis(); }
  bool isLeftBracket() const override { assert(false); return ExceptionLayoutNode<LayoutNode>::isLeftBracket(); }
  bool isRightBracket() const override { assert(false); return ExceptionLayoutNode<LayoutNode>::isRightBracket(); }
  bool isEmpty() const override { assert(false); return ExceptionLayoutNode<LayoutNode>::isEmpty(); }
  bool isMatrix() const override { assert(false); return ExceptionLayoutNode<LayoutNode>::isMatrix(); }
  bool hasUpperLeftIndex() const override { assert(false); return ExceptionLayoutNode<LayoutNode>::hasUpperLeftIndex(); }

  bool willAddChildAtIndex(LayoutNode * l, int * index, int * currentNumberOfChildren, LayoutCursor * cursor) override { assert(false); return ExceptionLayoutNode<LayoutNode>::willAddChildAtIndex(l, index, currentNumberOfChildren, cursor); }
  bool willAddSibling(LayoutCursor * cursor, LayoutNode * sibling, bool moveCursor) override { assert(false); return ExceptionLayoutNode<LayoutNode>::willAddSibling(cursor, sibling, moveCursor); }
  void willAddSiblingToEmptyChildAtIndex(int childIndex) override { assert(false); return ExceptionLayoutNode<LayoutNode>::willAddSiblingToEmptyChildAtIndex(childIndex); }
  bool willReplaceChild(LayoutNode * oldChild, LayoutNode * newChild, LayoutCursor * cursor, bool force) override { assert(false); return ExceptionLayoutNode<LayoutNode>::willReplaceChild(oldChild, newChild, cursor, force); }
  void didReplaceChildAtIndex(int index, LayoutCursor * cursor, bool force) override { assert(false); return ExceptionLayoutNode<LayoutNode>::didReplaceChildAtIndex(index, cursor, force); }
  bool willRemoveChild(LayoutNode * l, LayoutCursor * cursor, bool force) override { assert(false); return ExceptionLayoutNode<LayoutNode>::willRemoveChild(l, cursor, force); }
  void didRemoveChildAtIndex(int index, LayoutCursor * cursor, bool force) override { assert(false); return ExceptionLayoutNode<LayoutNode>::didRemoveChildAtIndex(index, cursor, force); }

  // TreeNode
  bool isUninitialized() const override { return true; }
  size_t size() const override { return sizeof(UninitializedLayoutNode); }
#if POINCARE_TREE_LOG
  virtual void logNodeName(std::ostream & stream) const override {
    stream << "UninitializedLayout";
  }
#endif

protected:
  // LayoutNode
  void moveCursorVertically(LayoutNode::VerticalDirection direction, LayoutCursor * cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited) override {
    assert(false);
    return ExceptionLayoutNode<LayoutNode>::moveCursorVertically(direction, cursor, shouldRecomputeLayout, equivalentPositionVisited);
  }
  KDCoordinate computeBaseline() override { assert(false); return ExceptionLayoutNode<LayoutNode>::computeBaseline(); }

private:
  void render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) override { assert(false); ExceptionLayoutNode<LayoutNode>::render(ctx, p, expressionColor, backgroundColor); }
};

}

#endif
