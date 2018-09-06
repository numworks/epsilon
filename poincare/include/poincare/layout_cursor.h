#ifndef POINCARE_LAYOUT_CURSOR_H
#define POINCARE_LAYOUT_CURSOR_H

#include <poincare/layout_reference.h>
#include <poincare/layout_node.h>

namespace Poincare {

class HorizontalLayoutNode;
class VerticalOffsetLayoutRef;

class LayoutCursor {
  friend class LayoutReference;
  friend class HorizontalLayoutNode;
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
    m_layoutRef(),
    m_position(Position::Right)
  {}

  LayoutCursor(LayoutRef layoutR, Position position = Position::Right) :
    m_layoutRef(layoutR.node()),
    m_position(position)
  {}

  LayoutCursor(LayoutNode * node, Position position = Position::Right) :
    m_layoutRef(node),
    m_position(position)
  {} //TODO make this private and friend class layout_node

  LayoutCursor clone() const {
    return LayoutCursor(m_layoutRef, m_position);
  }

  // Definition
  bool isDefined() const { return !m_layoutRef.isUninitialized(); }

  // Getters and setters
  LayoutRef layoutReference() { return m_layoutRef; }
  LayoutNode * layoutNode() { return m_layoutRef.node(); } // TODO  Make private + friend classes ?

  int layoutIdentifier() { return m_layoutRef.identifier(); }
  void setLayoutReference(LayoutRef r) {
    if (r != m_layoutRef) {
      m_layoutRef = r;
    }
  }
  void setLayoutNode(LayoutNode * n) {
    if (n != m_layoutRef.node()) {
      m_layoutRef = LayoutRef(n);
    }
  }
  void setTo(LayoutCursor * other) {
     m_layoutRef = other->layoutReference();
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
  void addXNTCharLayout();
  void insertText(const char * text);
  void addLayoutAndMoveCursor(LayoutRef l);
  bool showEmptyLayoutIfNeeded() { return privateShowHideEmptyLayoutIfNeeded(true); }
  bool hideEmptyLayoutIfNeeded() { return privateShowHideEmptyLayoutIfNeeded(false); }
  void performBackspace() { m_layoutRef.deleteBeforeCursor(this); }
  void clearLayout();

private:
  constexpr static KDCoordinate k_cursorHeight = 18;
  KDCoordinate layoutHeight();
  void privateAddEmptyPowerLayout(VerticalOffsetLayoutRef v);
  bool baseForNewPowerLayout();
  bool privateShowHideEmptyLayoutIfNeeded(bool show);
  LayoutRef m_layoutRef;
  Position m_position;
};

}

#endif
