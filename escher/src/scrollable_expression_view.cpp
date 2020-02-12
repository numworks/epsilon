#include <escher/scrollable_expression_view.h>
#include <poincare/layout.h>
#include <escher/metric.h>
#include <assert.h>

ScrollableExpressionView::ScrollableExpressionView(Responder * parentResponder, KDCoordinate leftRightMargin, KDCoordinate topBottomMargin, float horizontalAlignment, float verticalAlignment, KDColor textColor, KDColor backgroundColor) :
  ScrollableView(parentResponder, &m_expressionView, this),
  m_expressionView(horizontalAlignment, verticalAlignment, textColor, backgroundColor)
{
  setDecoratorType(ScrollView::Decorator::Type::Arrows);
  setMargins(
    topBottomMargin,
    leftRightMargin,
    topBottomMargin,
    leftRightMargin
  );
  setBackgroundColor(backgroundColor);
}

Poincare::Layout ScrollableExpressionView::layout() const {
  return m_expressionView.layout();
}

void ScrollableExpressionView::setLayout(Poincare::Layout layout) {
  m_expressionView.setLayout(layout);
}

void ScrollableExpressionView::setBackgroundColor(KDColor backgroundColor) {
  m_expressionView.setBackgroundColor(backgroundColor);
  ScrollableView::setBackgroundColor(backgroundColor);
}

void ScrollableExpressionView::setExpressionBackgroundColor(KDColor backgroundColor) {
  m_expressionView.setBackgroundColor(backgroundColor);
}
