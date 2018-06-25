#include "layout_cursor.h"
#include "layout_reference.h"
#include <stdio.h>

/* Comparison */

bool LayoutCursor::isEquivalentTo(LayoutCursor cursor) {
  assert(isDefined());
  assert(cursor.isDefined());
  return middleLeftPoint() == cursor.middleLeftPoint();
}

/* Position */

int LayoutCursor::middleLeftPoint() {
  int layoutOrigin = m_layoutPointer.absoluteOrigin();
  return layoutOrigin;
}

/* Move */

void LayoutCursor::moveLeft(bool * shouldRecomputeLayout) {
  m_layoutPointer.moveCursorLeft(this, shouldRecomputeLayout);
}

void LayoutCursor::moveRight(bool * shouldRecomputeLayout) {
  m_layoutPointer.moveCursorRight(this, shouldRecomputeLayout);
}

void LayoutCursor::moveAbove(bool * shouldRecomputeLayout) {
  m_layoutPointer.moveCursorUp(this, shouldRecomputeLayout);
}

void LayoutCursor::moveUnder(bool * shouldRecomputeLayout) {
  m_layoutPointer.moveCursorDown(this, shouldRecomputeLayout);
}


