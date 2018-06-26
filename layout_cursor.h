#ifndef LAYOUT_CURSOR_H
#define LAYOUT_CURSOR_H

#include "cursor.h"
#include "layout_reference.h"
#include "layout_node.h"
#include <stdio.h>

class LayoutCursor : public Cursor {
  template <typename T>
  friend class LayoutReference;
public:
  enum class Position {
    Left,
    Right
  };

  /* Debug */
  void log() {
    printf("Pointed Layout id %d, cursor position ", m_treeReference.identifier());
    if (m_position == Position::Left) {
      printf("Left");
    } else {
      printf("Right");
    }
    printf("\n");
  }

  /* Getters and setters */
  LayoutReference<LayoutNode> layoutReference() {
    return LayoutReference<LayoutNode>(m_treeReference.node());
  }
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
  LayoutCursor(TreeNode * node, Position position = Position::Right) :
    Cursor(node),
    m_position(position)
  {
  }
  Position m_position;
};

#endif
