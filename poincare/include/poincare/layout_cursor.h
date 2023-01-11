#ifndef POINCARE_LAYOUT_CURSOR_H
#define POINCARE_LAYOUT_CURSOR_H

#include <omg/enums.h>
#include <poincare/layout.h>

namespace Poincare {

class HorizontalLayoutNode;
class VerticalOffsetLayout;

class LayoutCursor final {
  friend class AutocompletedBracketPairLayoutNode;
  friend class BinomialCoefficientLayoutNode;
  friend class BracketLayoutNode;
  friend class BracketPairLayoutNode;
  friend class ConjugateLayoutNode;
  friend class DerivativeLayoutNode;
  friend class EmptyLayoutNode;
  friend class FirstOrderDerivativeLayoutNode;
  friend class FractionLayoutNode;
  friend class GridLayoutNode;
  friend class HigherOrderDerivativeLayoutNode;
  friend class HorizontalLayoutNode;
  friend class IntegralLayoutNode;
  friend class Layout;
  friend class LayoutNode;
  friend class LetterWithSubAndSuperscriptLayoutNode;
  friend class ListSequenceLayoutNode;
  friend class MatrixLayoutNode;
  friend class NthRootLayoutNode;
  friend class PiecewiseOperatorLayoutNode;
  friend class SequenceLayoutNode;
  friend class SquareBracketPairLayoutNode;
  friend class VerticalOffsetLayoutNode;
public:
  constexpr static KDCoordinate k_cursorWidth = 1;

  enum class Position {
    Left,
    Right
  };

  LayoutCursor() :
    m_position(Position::Right)
  {}

  LayoutCursor(Layout layoutR, Position position = Position::Right) :
    m_layout(layoutR.node()),
    m_position(position)
  {}

  // Definition
  bool isDefined() const { return !m_layout.isUninitialized(); }

  // Getters and setters
  Layout layout() { return m_layout; }
  LayoutNode * layoutNode() { return m_layout.node(); }

  int layoutIdentifier() { return m_layout.identifier(); }
  void setLayout(Layout r) {
    if (r != m_layout) {
      m_layout = r;
    }
  }
  void setTo(LayoutCursor * other) {
     m_layout = other->layout();
     m_position = other->position();
  }
  Position position() const { return m_position; }
  void setPosition(Position position) { m_position = position; }
  KDCoordinate cursorHeightWithoutSelection(KDFont::Size font);
  KDCoordinate baselineWithoutSelection(KDFont::Size font);


  /* Comparison */
  bool isEquivalentTo(LayoutCursor cursor);

  /* Position */
  KDPoint middleLeftPoint();

  /* Move */
  void move(OMG::Direction direction, bool * shouldRecomputeLayout, bool forSelection = false);
  void moveHorizontally(OMG::HorizontalDirection direction, bool * shouldRecomputeLayout, bool forSelection = false) {
    move(direction == OMG::HorizontalDirection::Left ? OMG::Direction::Left : OMG::Direction::Right, shouldRecomputeLayout, forSelection);
  }
  void moveLeft(bool * shouldRecomputeLayout, bool forSelection = false) {
    layoutNode()->moveCursorLeft(this, shouldRecomputeLayout, forSelection);
  }
  void moveRight(bool * shouldRecomputeLayout, bool forSelection = false) {
    layoutNode()->moveCursorRight(this, shouldRecomputeLayout, forSelection);
  }
  void moveAbove(bool * shouldRecomputeLayout, bool forSelection = false) {
    layoutNode()->moveCursorUp(this, shouldRecomputeLayout, false, forSelection);
  }
  void moveUnder(bool * shouldRecomputeLayout, bool forSelection = false) {
    layoutNode()->moveCursorDown(this, shouldRecomputeLayout, false, forSelection);
  }
  LayoutCursor cursorAtDirection(OMG::Direction direction, bool * shouldRecomputeLayout, bool forSelection = false, int step = 1);

  /* Select */
  void select(OMG::Direction direction, bool * shouldRecomputeLayout, Layout * selection);
  LayoutCursor selectAtDirection(OMG::Direction direction, bool * shouldRecomputeLayout, Layout * selection);

  /* Layout modification */
  void addEmptyExponentialLayout(Context * context);
  void addEmptyMatrixLayout(Context * context);
  void addEmptyPowerLayout(Context * context);
  void addEmptySquareRootLayout(Context * context);
  void addEmptySquarePowerLayout(Context * context);
  void addEmptyTenPowerLayout(Context * context);
  void addFractionLayoutAndCollapseSiblings(Context * context);
  void insertText(const char * text, Context * context, bool forceCursorRightOfText = false, bool forceCursorLeftOfText = false);
  void addLayoutAndMoveCursor(Layout l, Context * context, bool forceCursorRightOfLayout = false, bool withinBeautification = false);
  bool showEmptyLayoutIfNeeded() { return privateShowHideEmptyLayoutIfNeeded(true); }
  bool hideEmptyLayoutIfNeeded() { return privateShowHideEmptyLayoutIfNeeded(false); }
  void performBackspace() { m_layout.deleteBeforeCursor(this); }
  void clearLayout();

  bool isAtNumeratorOfEmptyFraction() const;

private:
  constexpr static KDCoordinate k_cursorHeight = 18;

  LayoutCursor(LayoutNode * node, Position position = Position::Right) :
    m_layout(node),
    m_position(position)
  {}

  void setLayoutNode(LayoutNode * n) {
    if (n->identifier() != m_layout.identifier()) {
      /* Compare the identifiers and not the nodes because m_layout might
       * temporarily be pointing to a GhostNode. In this case,
       * m_layout.node() would crash because of the assertion that the node
       * is not ghost. */
      m_layout = Layout(n);
    }
  }
  KDCoordinate layoutHeight(KDFont::Size font);
  void privateAddEmptyPowerLayout(VerticalOffsetLayout v);
  bool baseForNewPowerLayout();
  bool privateShowHideEmptyLayoutIfNeeded(bool show);
  void selectLeftRight(bool right, bool * shouldRecomputeLayout, Layout * selection);
  void selectUpDown(bool up, bool * shouldRecomputeLayout, Layout * selection);
  /* Return an uninitialized layout if the cursor is not inside a bracket pair,
   * touching one of the brackets. */
  Layout bracketsEncompassingCursor(Layout equivalentLayout) const;

  Layout m_layout;
  Position m_position;
};

}

#endif
