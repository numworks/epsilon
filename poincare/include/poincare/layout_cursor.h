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

  enum class MoveDirection {
    Left,
    Right,
    Up,
    Down
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
  Layout layoutReference() { return m_layout; }

  int layoutIdentifier() { return m_layout.identifier(); }
  void setLayout(Layout r) {
    if (r != m_layout) {
      m_layout = r;
    }
  }
  void setTo(LayoutCursor * other) {
     m_layout = other->layoutReference();
     m_position = other->position();
  }
  Position position() const { return m_position; }
  void setPosition(Position position) { m_position = position; }
  KDCoordinate cursorHeight();
  KDCoordinate baseline();


  /* Comparison */
  bool isEquivalentTo(LayoutCursor cursor);

  /* Position */
  KDPoint middleLeftPoint();

  /* Move */
  void move(MoveDirection direction, bool * shouldRecomputeLayout);
  void moveLeft(bool * shouldRecomputeLayout) {
    layoutNode()->moveCursorLeft(this, shouldRecomputeLayout);
  }
  void moveRight(bool * shouldRecomputeLayout) {
    layoutNode()->moveCursorRight(this, shouldRecomputeLayout);
  }
  void moveAbove(bool * shouldRecomputeLayout) {
    layoutNode()->moveCursorUp(this, shouldRecomputeLayout);
  }
  void moveUnder(bool * shouldRecomputeLayout) {
    layoutNode()->moveCursorDown(this, shouldRecomputeLayout);
  }
  LayoutCursor cursorAtDirection(MoveDirection direction, bool * shouldRecomputeLayout) {
    LayoutCursor result = clone();
    result.move(direction, shouldRecomputeLayout);
    return result;
  }

  /* Layout modification */
  void addEmptyExponentialLayout();
  void addEmptyMatrixLayout();
  void addEmptyPowerLayout();
  void addEmptySquareRootLayout();
  void addEmptySquarePowerLayout();
  void addEmptyTenPowerLayout();
  void addFractionLayoutAndCollapseSiblings();
  void addXNTCodePointLayout();
  void insertText(const char * text);
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
  Layout m_layout;
  Position m_position;
};

}

#endif
