#ifndef POINCARE_LAYOUT_CURSOR_H
#define POINCARE_LAYOUT_CURSOR_H

#include <poincare/layout.h>

namespace Poincare {

class HorizontalLayoutNode;
class VerticalOffsetLayout;

class LayoutCursor final {
  friend class Layout;
  friend class BinomialCoefficientLayoutNode;
  friend class BracketLayoutNode;
  friend class BracketPairLayoutNode;
  friend class ConjugateLayoutNode;
  friend class EmptyLayoutNode;
  friend class FractionLayoutNode;
  friend class GridLayoutNode;
  friend class HorizontalLayoutNode;
  friend class IntegralLayoutNode;
  friend class LayoutNode;
  friend class MatrixLayoutNode;
  friend class NthRootLayoutNode;
  friend class SequenceLayoutNode;
  friend class VerticalOffsetLayoutNode;
public:
  constexpr static KDCoordinate k_cursorWidth = 1;

  enum class Direction {
    Left,
    Up,
    Down,
    Right
  };

  enum class Position {
    Left,
    Right
  };

  LayoutCursor() :
    m_layout(),
    m_position(Position::Right)
  {}

  LayoutCursor(Layout layoutR, Position position = Position::Right) :
    m_layout(layoutR.node()),
    m_position(position)
  {}

  LayoutCursor clone() const {
    return LayoutCursor(m_layout, m_position);
  }

  // Definition
  bool isDefined() const { return !m_layout.isUninitialized(); }

  // Getters and setters
  Layout layout() { return m_layout; }

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
  KDCoordinate cursorHeightWithoutSelection();
  KDCoordinate baselineWithoutSelection();


  /* Comparison */
  bool isEquivalentTo(LayoutCursor cursor);

  /* Position */
  KDPoint middleLeftPoint();

  /* Move */
  void move(Direction direction, bool * shouldRecomputeLayout, bool forSelection = false);
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
  LayoutCursor cursorAtDirection(Direction direction, bool * shouldRecomputeLayout, bool forSelection = false, int step = 1);

  /* Select */
  void select(Direction direction, bool * shouldRecomputeLayout, Layout * selection);
  LayoutCursor selectAtDirection(Direction direction, bool * shouldRecomputeLayout, Layout * selection);

  /* Layout modification */
  void addEmptyExponentialLayout();
  void addEmptyMatrixLayout();
  void addEmptyPowerLayout();
  void addRoot();
  void addLog();
  void addEmptyLog();
  void addEmpty10Log();
  void addEmptyArgLog();
  void addEmptySquareRootLayout();
  void addEmptyRootLayout();
  void addEmptyArgSquareRootLayout();
  void addEmptySquarePowerLayout();
  void addEmptyTenPowerLayout();
  void addFractionLayoutAndCollapseSiblings();
  void addXNTCodePointLayout();
  void addMultiplicationPointLayout();
  void insertText(const char * text, bool forceCursorRightOfText = false);
  void addLayoutAndMoveCursor(Layout l);
  bool showEmptyLayoutIfNeeded() { return privateShowHideEmptyLayoutIfNeeded(true); }
  bool hideEmptyLayoutIfNeeded() { return privateShowHideEmptyLayoutIfNeeded(false); }
  void performBackspace() { m_layout.deleteBeforeCursor(this); }
  void clearLayout();

private:
  constexpr static KDCoordinate k_cursorHeight = 18;
  LayoutCursor(LayoutNode * node, Position position = Position::Right) :
    m_layout(node),
    m_position(position)
  {}
  LayoutNode * layoutNode() { return m_layout.node(); }
  void setLayoutNode(LayoutNode * n) {
    if (n->identifier() != m_layout.identifier()) {
      /* Compare the identifiers and not the nodes because m_layout might
       * temporarily be pointing to a GhostNode. In this case,
       * m_layout.node() would crash because of the assertion that the node
       * is not ghost. */
      m_layout = Layout(n);
    }
  }
  KDCoordinate layoutHeight();
  void privateAddEmptyPowerLayout(VerticalOffsetLayout v);
  bool baseForNewPowerLayout();
  bool privateShowHideEmptyLayoutIfNeeded(bool show);
  void selectLeftRight(bool right, bool * shouldRecomputeLayout, Layout * selection);
  void selectUpDown(bool up, bool * shouldRecomputeLayout, Layout * selection);
  Layout m_layout;
  Position m_position;
};

}

#endif
