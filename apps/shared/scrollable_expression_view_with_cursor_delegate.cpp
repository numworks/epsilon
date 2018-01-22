#include "scrollable_expression_view_with_cursor_delegate.h"

using namespace Poincare;

namespace Shared {

bool ScrollableExpressionViewWithCursorDelegate::scrollableExpressionViewWithCursorShouldFinishEditing(ScrollableExpressionViewWithCursor * scrollableExpressionViewWithCursor, Ion::Events::Event event) {
  return editableExpressionViewDelegateApp()->scrollableExpressionViewWithCursorShouldFinishEditing(scrollableExpressionViewWithCursor, event);
}

bool ScrollableExpressionViewWithCursorDelegate::scrollableExpressionViewWithCursorDidReceiveEvent(ScrollableExpressionViewWithCursor * scrollableExpressionViewWithCursor, Ion::Events::Event event) {
  return editableExpressionViewDelegateApp()->scrollableExpressionViewWithCursorDidReceiveEvent(scrollableExpressionViewWithCursor, event);
}

bool ScrollableExpressionViewWithCursorDelegate::scrollableExpressionViewWithCursorDidFinishEditing(ScrollableExpressionViewWithCursor * scrollableExpressionViewWithCursor, const char * text, Ion::Events::Event event) {
  return editableExpressionViewDelegateApp()->scrollableExpressionViewWithCursorDidFinishEditing(scrollableExpressionViewWithCursor, text, event);
}

bool ScrollableExpressionViewWithCursorDelegate::scrollableExpressionViewWithCursorDidAbortEditing(ScrollableExpressionViewWithCursor * scrollableExpressionViewWithCursor, const char * text) {
  return editableExpressionViewDelegateApp()->scrollableExpressionViewWithCursorDidAbortEditing(scrollableExpressionViewWithCursor, text);
}

void ScrollableExpressionViewWithCursorDelegate::scrollableExpressionViewWithCursorDidChangeSize(ScrollableExpressionViewWithCursor * scrollableExpressionViewWithCursor) {
  return editableExpressionViewDelegateApp()->scrollableExpressionViewWithCursorDidChangeSize(scrollableExpressionViewWithCursor);
}

Toolbox * ScrollableExpressionViewWithCursorDelegate::toolboxForScrollableExpressionViewWithCursor(ScrollableExpressionViewWithCursor * scrollableExpressionViewWithCursor) {
  return editableExpressionViewDelegateApp()->toolboxForScrollableExpressionViewWithCursor(scrollableExpressionViewWithCursor);
}

}
