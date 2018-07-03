#ifndef POINCARE_LAYOUT_CURSOR_H
#define POINCARE_LAYOUT_CURSOR_H

#include "layout_reference.h"
#include "layout_node.h"
#include <stdio.h>

namespace Poincare {

class LayoutCursor {
  template <typename T>
  friend class LayoutReference;
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

  /* Debug */
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

  bool isDefined() const { return m_layoutRef.isDefined(); }


  /* Getters and setters */
  LayoutRef layoutReference() { return m_layoutRef; }
  int layoutIdentifier() { return m_layoutRef.identifier(); }
  void setLayoutReference(LayoutRef r) { m_layoutRef = r; }
  void setLayoutNode(LayoutNode * n) {
    m_layoutRef.node()->release();
    m_layoutRef = LayoutRef(n);
  }
  Position position() const { return m_position; }
  void setPosition(Position position) { m_position = position; }
  KDCoordinate cursorHeight();
  KDCoordinate baseline();
  LayoutCursor clone() const {
    return LayoutCursor(m_layoutRef, m_position);
  }

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
  void clearLayout() {} //TODO
  void addFractionLayoutAndCollapseSiblings() {} //TODO
  void addEmptyExponentialLayout() {} //TODO
  void addEmptyPowerLayout() {} //TODO
  void addEmptySquareRootLayout() {} //TODO
  void addEmptySquarePowerLayout() {} //TODO
  void addEmptyTenPowerLayout() {} //TODO
  void addEmptyMatrixLayout() {} //TODO
  void insertText(const char * text) {} //TODO
  void performBackspace() {} //TODO
  bool showEmptyLayoutIfNeeded() { return false; } //TODO
  bool hideEmptyLayoutIfNeeded() { return false; } //TODO
  void addLayoutAndMoveCursor(LayoutRef l) {} //TODO

private:
  constexpr static KDCoordinate k_cursorHeight = 18;
  LayoutCursor(LayoutNode * node, Position position = Position::Right) :
    m_layoutRef(node),
    m_position(position)
  {}
  KDCoordinate layoutHeight();
  LayoutRef m_layoutRef;
  Position m_position;
};

}

#endif
