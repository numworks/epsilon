#ifndef POINCARE_ALLOCATION_FAILURE_LAYOUT_NODE_H
#define POINCARE_ALLOCATION_FAILURE_LAYOUT_NODE_H

#include <poincare/allocation_failure_node.h>
#include <poincare/layout_node.h>
#include <poincare/layout_reference.h>
#include <poincare/layout_cursor.h>

namespace Poincare {

template <typename T>
class AllocationFailureLayoutNode : public AllocationFailureNode<T> {
public:
  // LayoutNode
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override {
    assert(false);
    return 0;
    /*
    int descriptionLength = strlen(description()) + 1;
    return strlcpy(buffer, description(), bufferSize < descriptionLength ? bufferSize : descriptionLength);
    */
  }

  // Rendering
  void invalidAllSizesPositionsAndBaselines() override {}

  // Tree navigation
  void moveCursorLeft(LayoutCursor * cursor, bool * shouldRecomputeLayout) override {}
  void moveCursorRight(LayoutCursor * cursor, bool * shouldRecomputeLayout) override {}
  void moveCursorUp(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited = false) override {}
  void moveCursorDown(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited = false) override {}
  LayoutCursor equivalentCursor(LayoutCursor * cursor) override { return LayoutCursor(); }

  // Tree modification

  // Collapse
  bool shouldCollapseSiblingsOnLeft() const override { return false; }
  bool shouldCollapseSiblingsOnRight() const override { return false; }
  int leftCollapsingAbsorbingChildIndex() const override { assert(false); return 0; }
  int rightCollapsingAbsorbingChildIndex() const override { assert(false); return 0; }
  void didCollapseSiblings(LayoutCursor * cursor) override {}

  //User input
  void deleteBeforeCursor(LayoutCursor * cursor) override {}

  // Other
  LayoutNode * layoutToPointWhenInserting() override { return this; }
  bool hasText() const override { return false; }
  bool isCollapsable(int * numberOfOpenParenthesis, bool goingLeft) const override { return false; }
  bool canBeOmittedMultiplicationLeftFactor() const override { return false; }
  bool canBeOmittedMultiplicationRightFactor() const override { return false; }
  bool mustHaveLeftSibling() const override { return false; }
  bool isVerticalOffset() const override { return false; }
  bool isHorizontal() const override { return false; }
  bool isLeftParenthesis() const override { return false; }
  bool isRightParenthesis() const override { return false; }
  bool isLeftBracket() const override { return false; }
  bool isRightBracket() const override { return false; }
  bool isEmpty() const override { return false; }
  bool isMatrix() const override { return false; }
  bool hasUpperLeftIndex() const override { return false; }

  bool willAddChildAtIndex(LayoutNode * l, int * index, int * currentNumberOfChildren, LayoutCursor * cursor) override { return false; }
  bool willAddSibling(LayoutCursor * cursor, LayoutNode * sibling, bool moveCursor) override { return false; }
  void willAddSiblingToEmptyChildAtIndex(int childIndex) override {}
  bool willReplaceChild(LayoutNode * oldChild, LayoutNode * newChild, LayoutCursor * cursor, bool force) override { return false; }
  void didReplaceChildAtIndex(int index, LayoutCursor * cursor, bool force) override {}
  bool willRemoveChild(LayoutNode * l, LayoutCursor * cursor, bool force) override { return false; }
  void didRemoveChildAtIndex(int index, LayoutCursor * cursor, bool force) override {}

  // TreeNode
  size_t size() const override { return sizeof(AllocationFailureLayoutNode<T>); }
#if TREE_LOG
  const char * description() const override { return "AllocationFailureLayout";  }
#endif

protected:
  // LayoutNode
  void moveCursorVertically(LayoutNode::VerticalDirection direction, LayoutCursor * cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited) override {}
  KDSize computeSize() override { return KDSizeZero; }
  KDCoordinate computeBaseline() override { return 0; }
  KDPoint positionOfChild(LayoutNode * child) override {
    assert(false);
    return KDPointZero;
  }

private:
  void render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) override {}
};

/*
template <typename T>
class AllocationFailureLayoutRef : public LayoutReference {
public:
  AllocationFailedLayoutRef() : LayoutReference(TreePool::sharedPool()->createTreeNode<AllocationFailureLayoutNode<T>>()) {}
};
*/

}

#endif
