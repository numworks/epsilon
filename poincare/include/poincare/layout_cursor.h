#ifndef POINCARE_LAYOUT_CURSOR_H
#define POINCARE_LAYOUT_CURSOR_H

#include "layout_reference.h"
#include "layout_node.h"
#include <stdio.h>

namespace Poincare {

class HorizontalLayoutNode;

class LayoutCursor {
  template <typename T>
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
    m_layoutRef(nullptr),
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
  bool isDefined() const { return m_layoutRef.isDefined(); }

  // Getters and setters
  LayoutRef layoutReference() { return m_layoutRef; }
  LayoutNode * layoutNode() { return m_layoutRef.typedNode(); } // TODO  Make private + friend classes ?

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
  Position position() const { return m_position; }
  void setPosition(Position position) { m_position = position; }
  KDCoordinate cursorHeight();
  KDCoordinate baseline();


  /* Comparison */
  bool isEquivalentTo(LayoutCursor cursor);

  /* Position */
  KDPoint middleLeftPoint();

  /* Move */
  void move(MoveDirection direction, bool * shouldRecomputeLayout) {
    if (direction == MoveDirection::Left) {
      moveLeft(shouldRecomputeLayout);
    } else if (direction == MoveDirection::Right) {
      moveRight(shouldRecomputeLayout);
    } else if (direction == MoveDirection::Up) {
      moveAbove(shouldRecomputeLayout);
    } else if (direction == MoveDirection::Down) {
      moveUnder(shouldRecomputeLayout);
    } else {
      assert(false);
    }
  }
  void moveLeft(bool * shouldRecomputeLayout);
  void moveRight(bool * shouldRecomputeLayout);
  void moveAbove(bool * shouldRecomputeLayout);
  void moveUnder(bool * shouldRecomputeLayout);
  LayoutCursor cursorAtDirection(MoveDirection direction, bool * shouldRecomputeLayout) {
    LayoutCursor result = clone();
    result.move(direction, shouldRecomputeLayout);
    return result;
  }

  /* Layout modification */
  void addEmptyExponentialLayout() {} //TODO
  void addEmptyMatrixLayout() {} //TODO
  void addEmptyPowerLayout() {} //TODO
  void addEmptySquareRootLayout() {} //TODO
  void addEmptySquarePowerLayout() {} //TODO
  void addEmptyTenPowerLayout() {} //TODO
  void addFractionLayoutAndCollapseSiblings() {} //TODO
  void addXNTCharLayout() {} //TODO
  void insertText(const char * text);
  void addLayoutAndMoveCursor(LayoutRef l);
  bool showEmptyLayoutIfNeeded() { return privateShowHideEmptyLayoutIfNeeded(true); }
  bool hideEmptyLayoutIfNeeded() { return privateShowHideEmptyLayoutIfNeeded(false); }
  void performBackspace() { m_layoutRef.deleteBeforeCursor(this); }
  void clearLayout();

  // Debug
  void log() {
#if TREE_LOG
    printf("Pointed Layout id %d, cursor position ", m_layoutRef.identifier());
    if (m_position == Position::Left) {
      printf("Left");
    } else {
      printf("Right");
    }
    printf("\n");
#endif
  }

private:
  constexpr static KDCoordinate k_cursorHeight = 18;
  KDCoordinate layoutHeight();
  bool baseForNewPowerLayout() { return false; } //TODO
  bool privateShowHideEmptyLayoutIfNeeded(bool show);
  LayoutRef m_layoutRef;
  Position m_position;
};

}

#endif
