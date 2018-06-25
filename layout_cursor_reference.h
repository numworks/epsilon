#ifndef LAYOUT_CURSOR_REFERENCE_H
#define LAYOUT_CURSOR_REFERENCE_H

#include "layout_node.h"
#include "layout_reference.h"

class LayoutCursor;

template <typename T>
class LayoutCursorReference : public LayoutReference<T> {
public:
  LayoutCursorReference(LayoutReference<T> * r) :
    LayoutReference<T>(r->node())
  {
  }

  LayoutCursorReference(const LayoutCursorReference & r) :
    LayoutReference<T>(r.node())
  {
  }

  // Tree navigation

  /* We cannot have LayoutCursor cursorLeftOf(LayoutCursor cursor) because of
   * circular header dependency */
  virtual void moveCursorLeft(LayoutCursor * cursor, bool * shouldRecomputeLayout) { return this->node()->moveCursorLeft(cursor, shouldRecomputeLayout); }
  virtual void moveCursorRight(LayoutCursor * cursor, bool * shouldRecomputeLayout) { return this->node()->moveCursorRight(cursor, shouldRecomputeLayout); }
  virtual void moveCursorUp(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited = false) {}
  virtual void moveCursorDown(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited = false) {}
};

typedef LayoutCursorReference<LayoutNode> LayoutPointer;

#endif
