#ifndef ESCHER_SCROLLABLE_EXPRESSION_VIEW_WITH_CURSOR_DELEGATE_H
#define ESCHER_SCROLLABLE_EXPRESSION_VIEW_WITH_CURSOR_DELEGATE_H

#include <escher/toolbox.h>
#include <ion/events.h>

class ScrollableExpressionViewWithCursor;

class ScrollableExpressionViewWithCursorDelegate {
public:
  virtual bool scrollableExpressionViewWithCursorShouldFinishEditing(ScrollableExpressionViewWithCursor * scrollableExpressionViewWithCursor, Ion::Events::Event event) = 0;
  virtual bool scrollableExpressionViewWithCursorDidReceiveEvent(ScrollableExpressionViewWithCursor * scrollableExpressionViewWithCursor, Ion::Events::Event event) = 0;
  virtual bool scrollableExpressionViewWithCursorDidFinishEditing(ScrollableExpressionViewWithCursor * scrollableExpressionViewWithCursor, const char * text, Ion::Events::Event event) { return false; }
  virtual bool scrollableExpressionViewWithCursorDidAbortEditing(ScrollableExpressionViewWithCursor * scrollableExpressionViewWithCursor) { return false; }
  virtual void scrollableExpressionViewWithCursorDidChangeSize(ScrollableExpressionViewWithCursor * scrollableExpressionViewWithCursor) {}
  virtual Toolbox * toolboxForScrollableExpressionViewWithCursor(ScrollableExpressionViewWithCursor * scrollableExpressionViewWithCursor) = 0;
};

#endif
