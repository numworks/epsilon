#include "scrollable_expression_view.h"
#include <assert.h>
using namespace Poincare;

namespace Calculation {

ScrollableExpressionView::ScrollableExpressionView(Responder * parentResponder) :
  ScrollableView(parentResponder, &m_expressionView),
  m_expressionView(ExpressionView())
{
}

void ScrollableExpressionView::setExpression(ExpressionLayout * expressionLayout) {
  m_expressionView.setExpression(expressionLayout);
  layoutSubviews();
}

void ScrollableExpressionView::setBackgroundColor(KDColor backgroundColor) {
  m_expressionView.setBackgroundColor(backgroundColor);
}

KDSize ScrollableExpressionView::minimalSizeForOptimalDisplay() {
  return m_expressionView.minimalSizeForOptimalDisplay();
}

View * ScrollableExpressionView::view() {
  return &m_expressionView;
}

}
