#ifndef POINCARE_LAYOUT_NODE_H
#define POINCARE_LAYOUT_NODE_H

#include <kandinsky/color.h>
#include <kandinsky/context.h>
#include <kandinsky/point.h>
#include <kandinsky/size.h>
#include <escher/metric.h>
#include <poincare/tree_node.h>

namespace Poincare {

class Expression;
class LayoutCursor;
class Layout;

class LayoutNode : public TreeNode {
  friend class Layout;
public:
  enum class VerticalDirection {
    Up,
    Down
  };
  enum class Type : uint8_t {
    AbsoluteValueLayout,
    BinomialCoefficientLayout,
    BracketPairLayout,
    CeilingLayout,
    CodePointLayout,
    CombinedCodePointsLayout,
    CondensedSumLayout,
    ConjugateLayout,
    DerivativeLayout,
    EmptyLayout,
    FloorLayout,
    FractionLayout,
    GridLayout,
    HorizontalLayout,
    IntegralLayout,
    LeftCurlyBraceLayout,
    LeftParenthesisLayout,
    LeftSquareBracketLayout,
    ListSequenceLayout,
    MatrixLayout,
    LetterAWithSubAndSuperscriptLayout,
    LetterCWithSubAndSuperscriptLayout,
    NthRootLayout,
    ProductLayout,
    RightCurlyBraceLayout,
    RightParenthesisLayout,
    RightSquareBracketLayout,
    SumLayout,
    StringLayout,
    VectorNormLayout,
    VerticalOffsetLayout
  };

  // Constructor
  LayoutNode() :
    TreeNode(),
    m_frame(KDRectZero),
    m_baseline(0),
    m_baselined(false),
    m_positioned(false),
    m_sized(false),
    m_margin(false)
  {
  }

  /* Poor man's RTTI */
  virtual Type type() const = 0;

  // Comparison
  bool isIdenticalTo(Layout l);

  // Rendering
  void draw(KDContext * ctx, KDPoint p, KDColor expressionColor = KDColorBlack, KDColor backgroundColor = KDColorWhite, Layout * selectionStart = nullptr, Layout * selectionEnd = nullptr, KDColor selectionColor = KDColorRed);
  KDPoint absoluteOrigin();
  KDSize layoutSize();
  KDCoordinate baseline();
  void setMargin(bool hasMargin) { m_margin = hasMargin; }
  int leftMargin() { return m_margin ? Escher::Metric::OperatorHorizontalMargin : 0; }
  //TODO: invalid cache when tempering with hierarchy
  virtual void invalidAllSizesPositionsAndBaselines();
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode = Preferences::PrintFloatMode::Decimal, int numberOfSignificantDigits = 0) const override { assert(false); return 0; }

  // Tree
  LayoutNode * parent() const override { return static_cast<LayoutNode *>(TreeNode::parent()); }
  LayoutNode * childAtIndex(int i) const override { return static_cast<LayoutNode *>(TreeNode::childAtIndex(i)); }
  LayoutNode * root() override { return static_cast<LayoutNode *>(TreeNode::root()); }

  // Tree navigation
  virtual void moveCursorLeft(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool forSelection = false) = 0;
  virtual void moveCursorRight(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool forSelection = false) = 0;
  virtual void moveCursorUp(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited = false, bool forSelection = false) {
    moveCursorVertically(VerticalDirection::Up, cursor, shouldRecomputeLayout, equivalentPositionVisited, forSelection);
  }
  virtual void moveCursorDown(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited = false, bool forSelection = false) {
    moveCursorVertically(VerticalDirection::Down, cursor, shouldRecomputeLayout, equivalentPositionVisited, forSelection);
  }
  void moveCursorUpInDescendants(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool forSelection = false) {
    return moveCursorInDescendantsVertically(VerticalDirection::Up, cursor, shouldRecomputeLayout, forSelection);
  }
  void moveCursorDownInDescendants(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool forSelection = false) {
    return moveCursorInDescendantsVertically(VerticalDirection::Down, cursor, shouldRecomputeLayout, forSelection);
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
  virtual LayoutNode * layoutToPointWhenInserting(Expression * correspondingExpression);
  bool removeGraySquaresFromAllMatrixAncestors();
  bool removeGraySquaresFromAllMatrixChildren();
  bool addGraySquaresToAllMatrixAncestors();
  /* A layout has text if it is not empty and it is not an horizontal layout
   * with no child or with one child with no text. */
  virtual bool hasText() const { return true; }
  virtual bool isCollapsable(int * numberOfOpenParenthesis, bool goingLeft) const { return true; }
  /* isCollapsable is used when adding a sibling fraction: should the layout be
   * inserted in the numerator (or denominator)? For instance, 1+2|3-4 should
   * become 1+ 2/3 - 4 when pressing "Divide": a CodePointLayout is collapsable if
   * its char is not +, -, or *. */
  virtual bool canBeOmittedMultiplicationLeftFactor() const;
  virtual bool canBeOmittedMultiplicationRightFactor() const;
  /* canBeOmittedMultiplicationLeftFactor and RightFactor return true if the
   * layout, next to another layout, might be the factor of a multiplication
   * with an omitted multiplication sign. For instance, an absolute value layout
   * returns true, because |3|2 means |3|*2. A '+' CodePointLayout returns false,
   * because +'something' nevers means +*'something'. */
  virtual bool mustHaveLeftSibling() const { return false; }
  /* For now, mustHaveLeftSibling and isVerticalOffset behave the same, but code
   * is clearer with different names. */
  virtual bool isEmpty() const { return false; }
  virtual bool hasUpperLeftIndex() const { return false; }
  virtual Layout XNTLayout(int childIndex = -1) const;

  virtual bool willAddChildAtIndex(LayoutNode * l, int * index, int * currentNumberOfChildren, LayoutCursor * cursor) { return true; }
  virtual bool willAddSibling(LayoutCursor * cursor, LayoutNode * sibling, bool moveCursor) { return true; }
  virtual void willAddSiblingToEmptyChildAtIndex(int childIndex) {}
  virtual bool willReplaceChild(LayoutNode * oldChild, LayoutNode * newChild, LayoutCursor * cursor, bool force) { return true; }
  virtual void didReplaceChildAtIndex(int index, LayoutCursor * cursor, bool force) {}
  virtual bool willRemoveChild(LayoutNode * l, LayoutCursor * cursor, bool force);
  virtual void didRemoveChildAtIndex(int index, LayoutCursor * cursor, bool force) {}

  virtual Layout makeEditable();

protected:
  virtual bool protectedIsIdenticalTo(Layout l);

  // Tree navigation
  virtual void moveCursorVertically(VerticalDirection direction, LayoutCursor * cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited, bool forSelection);

  // Tree
  Direct<LayoutNode> children() { return Direct<LayoutNode>(this); }
  Direct<LayoutNode> childrenFromIndex(int i) { return Direct<LayoutNode>(this, i); }

  // Sizing and positioning
  virtual KDSize computeSize() = 0;
  virtual KDCoordinate computeBaseline() = 0;
  virtual KDPoint positionOfChild(LayoutNode * child) = 0;

  /* m_baseline is the signed vertical distance from the top of the layout to
   * the fraction bar of an hypothetical fraction sibling layout. If the top of
   * the layout is under that bar, the baseline is negative. */
  KDRect m_frame;
  KDCoordinate m_baseline;
  // These bools are aligned on 1 byte. They could be squashed into one uint8.
  bool m_baselined;
  bool m_positioned;
  bool m_sized;
  bool m_margin;

private:
  void moveCursorInDescendantsVertically(VerticalDirection direction, LayoutCursor * cursor, bool * shouldRecomputeLayout, bool forSelection);
  void scoreCursorInDescendantsVertically (
    VerticalDirection direction,
    LayoutCursor * cursor,
    bool * shouldRecomputeLayout,
    LayoutNode ** childResult,
    void * resultPosition,
    int * resultScore,
    bool forSelection);
  virtual void render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor, Layout * selectionStart = nullptr, Layout * selectionEnd = nullptr, KDColor selectionColor = KDColorRed) = 0;
  void changeGraySquaresOfAllMatrixRelatives(bool add, bool ancestors, bool * changedSquares);
};

}

#endif
