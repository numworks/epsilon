#include "expression_editor_view.h"
#include <assert.h>

namespace ExpressionEditor {

ExpressionEditorView::ExpressionEditorView(Responder * parentResponder, Poincare::ExpressionLayout * expressionLayout, Poincare::ExpressionLayoutCursor * cursor) :
  SolidColorView(KDColorWhite),
  m_scrollableExpressionViewWithCursor(parentResponder, expressionLayout, cursor)
{
}

void ExpressionEditorView::cursorPositionChanged() {
  m_scrollableExpressionViewWithCursor.expressionViewWithCursor()->cursorPositionChanged();
  m_scrollableExpressionViewWithCursor.scrollToCursor();
}

void ExpressionEditorView::layoutSubviews() {
  m_scrollableExpressionViewWithCursor.setFrame(KDRect(
    k_margin,
    k_margin,
    bounds().width() - 2 * k_margin,
    bounds().height() - 2 * k_margin));
  markRectAsDirty(bounds());
}

KDSize ExpressionEditorView::minimalSizeForOptimalDisplay() const {
  return m_scrollableExpressionViewWithCursor.minimalSizeForOptimalDisplay();
}


}

