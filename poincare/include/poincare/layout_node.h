#ifndef POINCARE_LAYOUT_NODE_H
#define POINCARE_LAYOUT_NODE_H

#include <poincare/serializable_node.h>
#include <kandinsky.h>
#include <ion/charset.h>

namespace Poincare {

class LayoutCursor;

class LayoutNode : public SerializableNode {
public:
  enum class VerticalDirection {
    Up,
    Down
  };
  enum class HorizontalDirection {
    Left,
    Right
  };

  // Constructor
  LayoutNode() :
    SerializableNode(),
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
  virtual void invalidAllSizesPositionsAndBaselines();

  // TreeNode
  static TreeNode * FailedAllocationStaticNode();
  TreeNode * failedAllocationStaticNode() override { return FailedAllocationStaticNode(); }

  LayoutNode * next() const override { return static_cast<LayoutNode *>(TreeNode::next()); }
  LayoutNode * nextSibling() const override { return static_cast<LayoutNode *>(TreeNode::nextSibling()); }

  // Tree
  LayoutNode * parent() const { return static_cast<LayoutNode *>(parentTree()); }
  LayoutNode * childAtIndex(int i) { return static_cast<LayoutNode *>(childTreeAtIndex(i)); }
  LayoutNode * root() { return static_cast<LayoutNode *>(rootTree()); }

  // Tree navigation
  virtual void moveCursorLeft(LayoutCursor * cursor, bool * shouldRecomputeLayout) = 0;
  virtual void moveCursorRight(LayoutCursor * cursor, bool * shouldRecomputeLayout) = 0;
  virtual void moveCursorUp(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited = false);
  virtual void moveCursorDown(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited = false);
  void moveCursorUpInDescendants(LayoutCursor * cursor, bool * shouldRecomputeLayout);
  void moveCursorDownInDescendants(LayoutCursor * cursor, bool * shouldRecomputeLayout);
  virtual LayoutCursor equivalentCursor(LayoutCursor * cursor);

  // Tree modification
  // Collapse
  virtual void collapseSiblings(LayoutCursor * cursor) {}
  // User input
  virtual void deleteBeforeCursor(LayoutCursor * cursor);

  // Other
  virtual LayoutNode * layoutToPointWhenInserting() {
    return numberOfChildren() > 0 ? childAtIndex(0) : this;
  }
  bool removeGreySquaresFromAllMatrixAncestors() { return changeGreySquaresOfAllMatrixAncestors(true); }
  bool addGreySquaresToAllMatrixAncestors() { return changeGreySquaresOfAllMatrixAncestors(false); }
  bool hasText() const;
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

  // TODO: put private
  virtual bool willAddSibling(LayoutCursor * cursor, LayoutNode * sibling, bool moveCursor) { return true; }
  virtual bool willReplaceChild(LayoutNode * oldChild, LayoutNode * newChild, LayoutCursor * cursor);
  virtual bool willRemoveChild(LayoutNode * l, LayoutCursor * cursor) { return true; }
  virtual void didRemoveChildAtIndex(int index, LayoutCursor * cursor, bool force) {}
protected:
  // Tree modification
  void collapseOnDirection(HorizontalDirection direction, int absorbingChildIndex);
  virtual void moveCursorVertically(VerticalDirection direction, LayoutCursor * cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited);
  LayoutNode * privateReplaceWith(LayoutNode * newChild);
  LayoutNode * privateReplaceWithAndMoveCursor(LayoutNode * newChild, LayoutCursor * cursor);

  Direct<LayoutNode> children() { return Direct<LayoutNode>(this); }

  // Sizing and positioning
  virtual void computeSize() = 0;
  virtual void computeBaseline() = 0;
  virtual KDPoint positionOfChild(LayoutNode * child) = 0;

  /* m_baseline is the signed vertical distance from the top of the layout to
   * the fraction bar of an hypothetical fraction sibling layout. If the top of
   * the layout is under that bar, the baseline is negative. */
  KDCoordinate m_baseline;
  KDRect m_frame;
  bool m_baselined;
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
