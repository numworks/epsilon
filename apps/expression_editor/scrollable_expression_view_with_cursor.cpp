#include "scrollable_expression_view_with_cursor.h"
#include <assert.h>

namespace ExpressionEditor {

ScrollableExpressionViewWithCursor::ScrollableExpressionViewWithCursor(Responder * parentResponder, Poincare::ExpressionLayout * expressionLayout, Poincare::ExpressionLayoutCursor * cursor) :
  ScrollableView(parentResponder, &m_expressionViewWithCursor, this),
  m_expressionViewWithCursor(expressionLayout, cursor)
{
}

KDSize ScrollableExpressionViewWithCursor::minimalSizeForOptimalDisplay() const {
  return m_expressionViewWithCursor.minimalSizeForOptimalDisplay();
}

void ScrollableExpressionViewWithCursor::scrollToCursor() {
  scrollToContentRect(m_expressionViewWithCursor.cursorRect(), true);
}

}

