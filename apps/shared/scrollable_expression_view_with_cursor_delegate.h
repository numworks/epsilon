#ifndef SHARED_SCROLLABLE_EXPRESSION_VIEW_WITH_CURSOR_DELEGATE_H
#define SHARED_SCROLLABLE_EXPRESSION_VIEW_WITH_CURSOR_DELEGATE_H

#include <escher/scrollable_expression_view_with_cursor_delegate.h>
#include "editable_expression_view_delegate_app.h"

namespace Shared {

class ScrollableExpressionViewWithCursorDelegate : public ::ScrollableExpressionViewWithCursorDelegate {
public:
  bool scrollableExpressionViewWithCursorShouldFinishEditing(ScrollableExpressionViewWithCursor * scrollableExpressionViewWithCursor, Ion::Events::Event event) override;
  bool scrollableExpressionViewWithCursorDidReceiveEvent(ScrollableExpressionViewWithCursor * scrollableExpressionViewWithCursor, Ion::Events::Event event) override;
  bool scrollableExpressionViewWithCursorDidFinishEditing(ScrollableExpressionViewWithCursor * scrollableExpressionViewWithCursor, const char * text, Ion::Events::Event event) override;
  bool scrollableExpressionViewWithCursorDidAbortEditing(ScrollableExpressionViewWithCursor * scrollableExpressionViewWithCursor, const char * text) override;
  void scrollableExpressionViewWithCursorDidChangeSize(ScrollableExpressionViewWithCursor * scrollableExpressionViewWithCursor) override;
  Toolbox * toolboxForScrollableExpressionViewWithCursor(ScrollableExpressionViewWithCursor * scrollableExpressionViewWithCursor) override;
private:
  virtual EditableExpressionViewDelegateApp * editableExpressionViewDelegateApp() = 0;
};

}

#endif
