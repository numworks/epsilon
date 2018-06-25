#ifndef LAYOUT_CURSOR_H
#define LAYOUT_CURSOR_H

#include "tree_pool.h"
#include "layout_cursor_reference.h"
#include "layout_node.h"
#include <stdio.h>

class LayoutCursor {
public:
  enum class Position {
    Left,
    Right
  };
  LayoutCursor(Layout * layoutPointer, Position position = Position::Right) :
    m_layoutPointer(layoutPointer),
    m_position(position)
  {
  }

  bool isDefined() const { return m_layoutPointer.isDefined(); }

  /* Debug */
  void log() {
    printf("Pointed Layout id %d, cursor position ", m_layoutPointer.identifier());
    if (m_position == Position::Left) {
      printf("Left");
    } else {
      printf("Right");
    }
    printf("\n");
  }

  /* Getters and setters */
  //int pointedLayoutIdentifier() const { return m_layoutIdentifier; }
  //void setPointedLayoutIdentifier(int layoutID) { m_layoutIdentifier = layoutID; }
  Position position() const { return m_position; }
  void setPosition(Position position) { m_position = position; }
  int cursorHeight() { return 1; } //TODO
  int baseline() { return 1; } //TODO

  /* Comparison */
  bool isEquivalentTo(LayoutCursor cursor);

  /* Position */
  int middleLeftPoint();

  /* Move */
  void moveLeft(bool * shouldRecomputeLayout);
  void moveRight(bool * shouldRecomputeLayout);
  void moveAbove(bool * shouldRecomputeLayout);
  void moveUnder(bool * shouldRecomputeLayout);
private:
  LayoutPointer m_layoutPointer;
  Position m_position;
};

#endif
