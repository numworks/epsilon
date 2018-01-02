#include "expression_editor_view.h"
#include <assert.h>

namespace ExpressionEditor {

ExpressionEditorView::ExpressionEditorView(Responder * parentResponder, Poincare::ExpressionLayout * expressionLayout, Poincare::ExpressionLayoutCursor * cursor) :
  SolidColorView(KDColorWhite),
  m_scrollableExpressionViewWithCursor(parentResponder, expressionLayout, cursor)
{
  m_serializerTextView.setText("Hello");
}

void ExpressionEditorView::cursorPositionChanged() {
  m_scrollableExpressionViewWithCursor.expressionViewWithCursor()->cursorPositionChanged();
  m_scrollableExpressionViewWithCursor.scrollToCursor();
}

void ExpressionEditorView::setText(const char * text) {
  m_serializerTextView.setText(text);
}

View * ExpressionEditorView::subviewAtIndex(int index) {
  assert(index >= 0 && index < 2);
  if (index == 0) {
    return &m_scrollableExpressionViewWithCursor;
  }
  return &m_serializerTextView;
}

void ExpressionEditorView::layoutSubviews() {
  m_serializerTextView.setFrame(KDRect(0, 0, bounds().width(), 20));
  m_scrollableExpressionViewWithCursor.setFrame(KDRect(
    k_margin,
    k_margin,
    bounds().width() - 2 * k_margin,
    bounds().height() - 2 * k_margin));
  markRectAsDirty(bounds());
}

KDSize ExpressionEditorView::minimalSizeForOptimalDisplay() const {
  return KDSize(300, 220);
}


}

