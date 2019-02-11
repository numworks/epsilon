#include "scrollable_expression_view.h"
#include <assert.h>
using namespace Poincare;

namespace Calculation {

ScrollableExpressionView::ScrollableExpressionView(Responder * parentResponder) :
  ScrollableView(parentResponder, &m_expressionView, this),
  m_expressionView()
{
  setDecoratorType(ScrollView::Decorator::Type::Arrows);
  setMargins(0, Metric::HistoryHorizontalMargin, 0, Metric::HistoryHorizontalMargin);
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
