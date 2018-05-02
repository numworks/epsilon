#include <escher/expression_layout_field.h>
#include <kandinsky/rect.h>
#include <assert.h>

using namespace Poincare;

ExpressionLayoutField::ContentView::ContentView(ExpressionLayout * expressionLayout, KDColor backgroundColor) :
  m_cursor(expressionLayout, ExpressionLayoutCursor::Position::Right),
  m_expressionView(0.0f, 0.5f, KDColorBlack, backgroundColor),
  m_cursorView(),
  m_isEditing(false)
{
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

void ExpressionLayoutField::ContentView::clearLayout() {
  m_cursor.clearLayout();
}

KDSize ExpressionLayoutField::ContentView::minimalSizeForOptimalDisplay() const {
  KDSize evSize = m_expressionView.minimalSizeForOptimalDisplay();
  return KDSize(evSize.width() + ExpressionLayoutCursor::k_cursorWidth, evSize.height());
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
  ExpressionLayout * pointedLayout = m_cursor.pointedExpressionLayout();
  ExpressionLayoutCursor::Position cursorPosition = m_cursor.position();
  ExpressionLayoutCursor eqCursor = pointedLayout->equivalentCursor(m_cursor);
  if (pointedLayout->hasChild(eqCursor.pointedExpressionLayout())) {
    pointedLayout = eqCursor.pointedExpressionLayout();
    cursorPosition = eqCursor.position();
  }
  KDPoint cursoredExpressionViewOrigin = pointedLayout->absoluteOrigin();
  KDCoordinate cursorX = expressionViewOrigin.x() + cursoredExpressionViewOrigin.x();
  if (cursorPosition == ExpressionLayoutCursor::Position::Right) {
    cursorX += pointedLayout->size().width();
  }
  KDPoint cursorTopLeftPosition(cursorX, expressionViewOrigin.y() + cursoredExpressionViewOrigin.y() + pointedLayout->baseline() - m_cursor.baseline());
  m_cursorView.setFrame(KDRect(cursorTopLeftPosition, ExpressionLayoutCursor::k_cursorWidth, m_cursor.cursorHeight()));
}
