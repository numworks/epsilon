#ifndef POINCARE_LAYOUT_NODE_H
#define POINCARE_LAYOUT_NODE_H

#include <kandinsky/color.h>
#include <kandinsky/context.h>
#include <kandinsky/point.h>
#include <kandinsky/size.h>
#include <escher/metric.h>
#include <omg/enums.h>
#include <omg/directions.h>
#include <poincare/tree_node.h>

namespace Poincare {

class Layout;
class LayoutSelection;

class LayoutNode : public TreeNode {
  friend class Layout;
public:
  enum class Type : uint8_t {
    AbsoluteValueLayout,
    BinomialCoefficientLayout,
    BracketPairLayout,
    CeilingLayout,
    CodePointLayout,
    CombinedCodePointsLayout,
    CondensedSumLayout,
    ConjugateLayout,
    CurlyBraceLayout,
    FirstOrderDerivativeLayout,
    FloorLayout,
    FractionLayout,
    HigherOrderDerivativeLayout,
    HorizontalLayout,
    IntegralLayout,
    LetterAWithSubAndSuperscriptLayout,
    LetterCWithSubAndSuperscriptLayout,
    ListSequenceLayout,
    MatrixLayout,
    NthRootLayout,
    PiecewiseOperatorLayout,
    ParenthesisLayout,
    ProductLayout,
    StringLayout,
    SumLayout,
    VectorNormLayout,
    VerticalOffsetLayout
  };

  // Constructor
  LayoutNode() :
    TreeNode(),
    m_frame(KDRectZero),
    m_baseline(0),
    m_flags({
      .m_baselined = false,
      .m_positioned = false,
      .m_sized = false,
      .m_margin = false,
      .m_lockMargin = false,
      .m_baselineFontSize = KDFont::Size::Small,
      .m_positionFontSize = KDFont::Size::Small,
      .m_sizeFontSize = KDFont::Size::Small,
    })
  {
  }

  /* Poor man's RTTI */
  virtual Type type() const = 0;
  bool isHorizontal() const { return type() == Type::HorizontalLayout; }

  // Comparison
  bool isIdenticalTo(Layout l, bool makeEditable = false);

  // Rendering
  void draw(KDContext * ctx, KDPoint p, KDFont::Size font, KDColor expressionColor, KDColor backgroundColor, LayoutSelection selection, KDColor selectionColor = KDColorRed);
  KDPoint absoluteOrigin(KDFont::Size font) { return absoluteOriginWithMargin(font).translatedBy(KDPoint(leftMargin(), 0)); }
  KDSize layoutSize(KDFont::Size font);
  KDCoordinate baseline(KDFont::Size font);
  void setMargin(bool hasMargin) { m_flags.m_margin = hasMargin; }
  void lockMargin(bool lock) { m_flags.m_lockMargin = lock; }
  int leftMargin() const { return m_flags.m_margin ? Escher::Metric::OperatorHorizontalMargin : 0; }
  bool marginIsLocked() const { return m_flags.m_lockMargin; }

  //TODO: invalid cache when tempering with hierarchy
  virtual void invalidAllSizesPositionsAndBaselines();
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode = Preferences::PrintFloatMode::Decimal, int numberOfSignificantDigits = 0) const override { assert(false); return 0; }

  // Tree
  LayoutNode * parent() const { return static_cast<LayoutNode *>(TreeNode::parent()); }
  LayoutNode * childAtIndex(int i) const {
    assert(i >= 0 && i < numberOfChildren());
    return static_cast<LayoutNode *>(TreeNode::childAtIndex(i));
  }
  LayoutNode * root() { return static_cast<LayoutNode *>(TreeNode::root()); }

  // Cursor navigation
  constexpr static int k_outsideIndex = -1;
  constexpr static int k_cantMoveIndex = -2;
  // Default implementation only handles cases of 0 or 1 child
  virtual int indexOfNextChildToPointToAfterHorizontalCursorMove(OMG::HorizontalDirection direction, int currentIndex, bool * shouldRedrawLayout);
  enum class PositionInLayout : uint8_t {
    Left,
    Middle,
    Right
  };
  virtual int indexOfNextChildToPointToAfterVerticalCursorMove(OMG::VerticalDirection direction, int currentIndex, PositionInLayout positionAtCurrentIndex, bool * shouldRedrawLayout);

  // Cursor deletion
  enum class DeletionMethod {
    DeleteLayout,
    DeleteParent,
    MoveLeft,
    FractionDenominatorDeletion,
    BinomialCoefficientMoveFromKtoN,
    GridLayoutMoveToUpperRow,
    GridLayoutDeleteColumnAndRow,
    GridLayoutDeleteColumn,
    GridLayoutDeleteRow,
    AutocompletedBracketPairMakeTemporary
  };
  virtual DeletionMethod deletionMethodForCursorLeftOfChild(int childIndex) const;
  static DeletionMethod StandardDeletionMethodForLayoutContainingArgument(int childIndex, int argumentIndex);

  // Cursor insertion
  virtual int indexOfChildToPointToWhenInserting();

  bool isEmpty() const { return isHorizontal() && numberOfChildren() == 0;}
  /* isCollapsable is used when adding a sibling fraction: should the layout be
   * inserted in the numerator (or denominator)? For instance, 1+2|3-4 should
   * become 1+ 2/3 - 4 when pressing "Divide": a CodePointLayout is collapsable if
   * its char is not +, -, or *. */
  virtual bool isCollapsable(int * numberOfOpenParenthesis, bool goingLeft) const { return true; }
  virtual bool canBeOmittedMultiplicationLeftFactor() const;
  virtual bool canBeOmittedMultiplicationRightFactor() const;
  /* canBeOmittedMultiplicationLeftFactor and RightFactor return true if the
   * layout, next to another layout, might be the factor of a multiplication
   * with an omitted multiplication sign. For instance, an absolute value layout
   * returns true, because |3|2 means |3|*2. A '+' CodePointLayout returns false,
   * because +'something' nevers means +*'something'. */
  virtual bool hasUpperLeftIndex() const { return false; }
  virtual Layout XNTLayout(int childIndex = -1) const;

  virtual Layout makeEditable();

  bool createGraySquaresAfterEnteringGrid(Layout layoutToExclude);
  bool deleteGraySquaresBeforeLeavingGrid(Layout layoutToExclude);

protected:
  virtual bool protectedIsIdenticalTo(Layout l);

  // Tree
  Direct<LayoutNode> children() { return Direct<LayoutNode>(this); }
  Direct<LayoutNode> childrenFromIndex(int i) { return Direct<LayoutNode>(this, i); }

  // Sizing and positioning
  virtual KDSize computeSize(KDFont::Size font) = 0;
  virtual KDCoordinate computeBaseline(KDFont::Size font) = 0;
  virtual KDPoint positionOfChild(LayoutNode * child, KDFont::Size font) = 0;

private:
  KDPoint absoluteOriginWithMargin(KDFont::Size font);
  virtual void render(KDContext * ctx, KDPoint p, KDFont::Size font, KDColor expressionColor, KDColor backgroundColor) = 0;
  bool changeGraySquaresOfAllGridRelatives(bool add, bool ancestors, Layout layoutToExclude);

  KDRect m_frame;
  /* m_baseline is the signed vertical distance from the top of the layout to
   * the fraction bar of an hypothetical fraction sibling layout. If the top of
   * the layout is under that bar, the baseline is negative. */
  KDCoordinate m_baseline;
  /* Squash multiple bool member variables into a packed struct. Taking
   * advantage of LayoutNode's data structure having room for many more booleans
   */
  struct Flags {
    bool m_baselined: 1;
    bool m_positioned: 1;
    bool m_sized: 1;
    bool m_margin: 1;
    bool m_lockMargin: 1;
    KDFont::Size m_baselineFontSize: 1;
    KDFont::Size m_positionFontSize: 1;
    KDFont::Size m_sizeFontSize: 1;
  };
  Flags m_flags;
};

}

#endif
