#include "expression_view_with_cursor.h"
#include <kandinsky/rect.h>

using namespace Poincare;

namespace ExpressionEditor {

ExpressionViewWithCursor::ExpressionViewWithCursor(ExpressionLayout * expressionLayout, ExpressionLayoutCursor * cursor) :
  m_cursor(cursor)
{
  m_expressionView.setExpressionLayout(expressionLayout);
}

void ExpressionViewWithCursor::cursorPositionChanged() {
  layoutCursorSubview();
}

KDRect ExpressionViewWithCursor::cursorRect() {
  return m_cursorView.frame();
}

KDSize ExpressionViewWithCursor::minimalSizeForOptimalDisplay() const {
  return m_expressionView.minimalSizeForOptimalDisplay();
}

View * ExpressionViewWithCursor::subviewAtIndex(int index) {
  assert(index >= 0 && index < 2);
  View * m_views[] = {&m_expressionView, &m_cursorView};
  return m_views[index];
}

void ExpressionViewWithCursor::layoutSubviews() {
  m_expressionView.setFrame(bounds());
  layoutCursorSubview();
}

void ExpressionViewWithCursor::layoutCursorSubview() {
  KDPoint expressionViewOrigin = m_expressionView.drawingOrigin();
  KDPoint cursoredExpressionViewOrigin = m_cursor->pointedExpressionLayout()->absoluteOrigin();
  KDCoordinate cursorX = expressionViewOrigin.x() + cursoredExpressionViewOrigin.x();
  if (m_cursor->position() == ExpressionLayoutCursor::Position::Right) {
    cursorX += m_cursor->pointedExpressionLayout()->size().width();
  } else if (m_cursor->position() == ExpressionLayoutCursor::Position::Inside) {
    cursorX += m_cursor->positionInside() * KDText::charSize().width();
  }
  KDPoint cursorTopLeftPosition(cursorX, expressionViewOrigin.y() + cursoredExpressionViewOrigin.y() + m_cursor->pointedExpressionLayout()->baseline()-k_cursorHeight/2);
  m_cursorView.setFrame(KDRect(cursorTopLeftPosition, 1, k_cursorHeight));
}

}
