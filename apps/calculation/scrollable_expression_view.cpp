#include "scrollable_expression_view.h"
#include <assert.h>
using namespace Poincare;

namespace Calculation {

ScrollableExpressionView::ScrollableExpressionView(Responder * parentResponder) :
  ScrollableView(parentResponder, &m_expressionView, this),
  m_expressionView()
{
  setDecoratorType(ScrollView::Decorator::Type::Arrows);
  setMargins(
    Metric::CommonSmallMargin,
    Metric::CommonLargeMargin,
    Metric::CommonSmallMargin,
    Metric::CommonLargeMargin
  );
}

Poincare::Layout ScrollableExpressionView::layout() const {
  return m_expressionView.layout();
}

void ScrollableExpressionView::setLayout(Layout layout) {
  m_expressionView.setLayout(layout);
}

void ScrollableExpressionView::setBackgroundColor(KDColor backgroundColor) {
  m_expressionView.setBackgroundColor(backgroundColor);
  ScrollableView::setBackgroundColor(backgroundColor);
}

void ScrollableExpressionView::setExpressionBackgroundColor(KDColor backgroundColor) {
  m_expressionView.setBackgroundColor(backgroundColor);
}

}
