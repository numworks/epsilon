#include <escher/expression_layout_field.h>
#include <kandinsky/rect.h>
#include <assert.h>

using namespace Poincare;

ExpressionLayoutField::ContentView::ContentView(ExpressionLayout * expressionLayout) :
  m_cursor(),
  m_expressionView(),
  m_cursorView(),
  m_isEditing(false)
{
  m_cursor.setPointedExpressionLayout(expressionLayout);
  m_cursor.setPosition(ExpressionLayoutCursor::Position::Right);
  m_expressionView.setExpressionLayout(expressionLayout);
}

void ExpressionLayoutField::ContentView::setEditing(bool isEditing) {
  m_isEditing = isEditing;
  markRectAsDirty(bounds());
  layoutSubviews();
}

void ExpressionLayoutField::ContentView::cursorPositionChanged() {
  layoutCursorSubview();
}

KDRect ExpressionLayoutField::ContentView::cursorRect() {
  return m_cursorView.frame();
}

KDSize ExpressionLayoutField::ContentView::minimalSizeForOptimalDisplay() const {
  KDSize expressionViewSize = m_expressionView.minimalSizeForOptimalDisplay();
  KDSize cursorSize = isEditing() ? m_cursorView.minimalSizeForOptimalDisplay() : KDSizeZero;
  KDCoordinate resultWidth = expressionViewSize.width() + cursorSize.width();
  KDCoordinate resultHeight = expressionViewSize.height() + cursorSize.height()/2;
  return KDSize(resultWidth, resultHeight);
}

View * ExpressionLayoutField::ContentView::subviewAtIndex(int index) {
  assert(index >= 0 && index < 2);
  View * m_views[] = {&m_expressionView, &m_cursorView};
  return m_views[index];
}

void ExpressionLayoutField::ContentView::layoutSubviews() {
  m_expressionView.setFrame(bounds());
  layoutCursorSubview();
}

void ExpressionLayoutField::ContentView::layoutCursorSubview() {
  if (!m_isEditing) {
    m_cursorView.setFrame(KDRectZero);
    return;
  }
  KDPoint expressionViewOrigin = m_expressionView.absoluteDrawingOrigin();
  ExpressionLayout * pointedLayout = m_cursor.pointedExpressionLayoutEquivalentChild();
  KDPoint cursoredExpressionViewOrigin = pointedLayout->absoluteOrigin();
  KDCoordinate cursorX = expressionViewOrigin.x() + cursoredExpressionViewOrigin.x();
  if (m_cursor.position() == ExpressionLayoutCursor::Position::Right) {
    cursorX += pointedLayout->size().width();
  }
  KDPoint cursorTopLeftPosition(cursorX, expressionViewOrigin.y() + cursoredExpressionViewOrigin.y() + pointedLayout->baseline() - m_cursor.baseline());
  m_cursorView.setFrame(KDRect(cursorTopLeftPosition, 1, m_cursor.cursorHeight()));
}
