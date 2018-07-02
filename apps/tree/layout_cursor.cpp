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
  int layoutOrigin = layoutReference().absoluteOrigin();
  return layoutOrigin;
}

/* Move */

void LayoutCursor::moveLeft(bool * shouldRecomputeLayout) {
  layoutReference().typedNode()->moveCursorLeft(this, shouldRecomputeLayout);
}

void LayoutCursor::moveRight(bool * shouldRecomputeLayout) {
  layoutReference().typedNode()->moveCursorRight(this, shouldRecomputeLayout);
}

void LayoutCursor::moveAbove(bool * shouldRecomputeLayout) {
  layoutReference().typedNode()->moveCursorUp(this, shouldRecomputeLayout);
}

void LayoutCursor::moveUnder(bool * shouldRecomputeLayout) {
  layoutReference().typedNode()->moveCursorDown(this, shouldRecomputeLayout);
}


