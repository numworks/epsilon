#ifndef POINCARE_LAYOUT_NODE_H
#define POINCARE_LAYOUT_NODE_H

#include <poincare/tree_node.h>
#include <kandinsky.h>
#include <ion/charset.h>

namespace Poincare {

class LayoutCursor;
class Layout;

class LayoutNode : public TreeNode {
  friend class Layout;
public:
  enum class VerticalDirection {
    Up,
    Down
  };

  // Constructor
  LayoutNode() :
    TreeNode(),
    m_baseline(0),
    m_frame(KDRectZero),
    m_baselined(false),
    m_positioned(false),
    m_sized(false)
  {
  }

  // Rendering
  void draw(KDContext * ctx, KDPoint p, KDColor expressionColor = KDColorBlack, KDColor backgroundColor = KDColorWhite);
  KDPoint origin();
  KDPoint absoluteOrigin();
  KDSize layoutSize();
  KDCoordinate baseline();
  //TODO: invalid cache when tempering with hierarchy
  virtual void invalidAllSizesPositionsAndBaselines();

  // Tree
  LayoutNode * parent() const override { return static_cast<LayoutNode *>(TreeNode::parent()); }
  LayoutNode * childAtIndex(int i) const override { return static_cast<LayoutNode *>(TreeNode::childAtIndex(i)); }
  LayoutNode * root() override { return static_cast<LayoutNode *>(TreeNode::root()); }

  // Tree navigation
  virtual void moveCursorLeft(LayoutCursor * cursor, bool * shouldRecomputeLayout) = 0;
  virtual void moveCursorRight(LayoutCursor * cursor, bool * shouldRecomputeLayout) = 0;
  virtual void moveCursorUp(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited = false) {
    moveCursorVertically(VerticalDirection::Up, cursor, shouldRecomputeLayout, equivalentPositionVisited);
  }
  virtual void moveCursorDown(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited = false) {
    moveCursorVertically(VerticalDirection::Down, cursor, shouldRecomputeLayout, equivalentPositionVisited);
  }
  void moveCursorUpInDescendants(LayoutCursor * cursor, bool * shouldRecomputeLayout) {
    return moveCursorInDescendantsVertically(VerticalDirection::Up, cursor, shouldRecomputeLayout);
  }
  void moveCursorDownInDescendants(LayoutCursor * cursor, bool * shouldRecomputeLayout) {
    return moveCursorInDescendantsVertically(VerticalDirection::Down, cursor, shouldRecomputeLayout);
  }
  virtual LayoutCursor equivalentCursor(LayoutCursor * cursor);

  // Tree modification
  // Collapse
  virtual bool shouldCollapseSiblingsOnLeft() const { return false; }
  virtual bool shouldCollapseSiblingsOnRight() const { return false; }
  virtual int leftCollapsingAbsorbingChildIndex() const { return 0; }
  virtual int rightCollapsingAbsorbingChildIndex() const { return 0; }
  virtual void didCollapseSiblings(LayoutCursor * cursor) {}

  // User input
  virtual void deleteBeforeCursor(LayoutCursor * cursor);

  // Other
  virtual LayoutNode * layoutToPointWhenInserting() {
    return numberOfChildren() > 0 ? childAtIndex(0) : this;
  }
  bool removeGreySquaresFromAllMatrixAncestors() { return changeGreySquaresOfAllMatrixAncestors(false); }
  bool addGreySquaresToAllMatrixAncestors() { return changeGreySquaresOfAllMatrixAncestors(true); }
  /* A layout has text if it is not empty and it is not an horizontal layout
   * with no child or with one child with no text. */
  virtual bool hasText() const { return true; }
  virtual bool isCollapsable(int * numberOfOpenParenthesis, bool goingLeft) const { return true; }
  /* isCollapsable is used when adding a sibling fraction: should the layout be
   * inserted in the numerator (or denominator)? For instance, 1+2|3-4 should
   * become 1+ 2/3 - 4 when pressing "Divide": a CharLayout is collapsable if
   * its char is not +, -, or *. */
  virtual bool canBeOmittedMultiplicationLeftFactor() const;
  virtual bool canBeOmittedMultiplicationRightFactor() const;
  /* canBeOmittedMultiplicationLeftFactor and RightFactor return true if the
   * layout, next to another layout, might be the factor of a multiplication
   * with an omitted multiplication sign. For instance, an absolute value layout
   * returns true, because |3|2 means |3|*2. A '+' CharLayout returns false,
   * because +'something' nevers means +*'something'. */
  virtual bool mustHaveLeftSibling() const { return false; }
  virtual bool isVerticalOffset() const { return false; }
  /* For now, mustHaveLeftSibling and isVerticalOffset behave the same, but code
   * is clearer with different names. */
  virtual bool isHorizontal() const { return false; }
  virtual bool isLeftParenthesis() const { return false; }
  virtual bool isRightParenthesis() const { return false; }
  virtual bool isLeftBracket() const { return false; }
  virtual bool isRightBracket() const { return false; }
  virtual bool isEmpty() const { return false; }
  virtual bool isMatrix() const { return false; }
  virtual bool hasUpperLeftIndex() const { return false; }
  virtual char XNTChar() const {
    LayoutNode * p = parent();
    return p == nullptr ? Ion::Charset::Empty : p->XNTChar();
  }

  virtual bool willAddChildAtIndex(LayoutNode * l, int * index, int * currentNumberOfChildren, LayoutCursor * cursor) { return true; }
  virtual bool willAddSibling(LayoutCursor * cursor, LayoutNode * sibling, bool moveCursor) { return true; }
  virtual void willAddSiblingToEmptyChildAtIndex(int childIndex) {}
  virtual bool willReplaceChild(LayoutNode * oldChild, LayoutNode * newChild, LayoutCursor * cursor, bool force) { return true; }
  virtual void didReplaceChildAtIndex(int index, LayoutCursor * cursor, bool force) {}
  virtual bool willRemoveChild(LayoutNode * l, LayoutCursor * cursor, bool force);
  virtual void didRemoveChildAtIndex(int index, LayoutCursor * cursor, bool force) {}

protected:
  // Tree navigation
  virtual void moveCursorVertically(VerticalDirection direction, LayoutCursor * cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited);

  // Tree
  Direct<LayoutNode> children() { return Direct<LayoutNode>(this); }

  // Sizing and positioning
  virtual KDSize computeSize() = 0;
  virtual KDCoordinate computeBaseline() = 0;
  virtual KDPoint positionOfChild(LayoutNode * child) = 0;

  /* m_baseline is the signed vertical distance from the top of the layout to
   * the fraction bar of an hypothetical fraction sibling layout. If the top of
   * the layout is under that bar, the baseline is negative. */
  KDCoordinate m_baseline;
  KDRect m_frame;
  bool m_baselined; // TODO Do not use so much space for 3 bools
  bool m_positioned;
  bool m_sized;
private:
  void moveCursorInDescendantsVertically(VerticalDirection direction, LayoutCursor * cursor, bool * shouldRecomputeLayout);
  void scoreCursorInDescendantsVertically (
    VerticalDirection direction,
    LayoutCursor * cursor,
    bool * shouldRecomputeLayout,
    LayoutNode ** childResult,
    void * resultPosition,
    int * resultScore);
  virtual void render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) = 0;
  bool changeGreySquaresOfAllMatrixAncestors(bool add);
};

}

#endif
