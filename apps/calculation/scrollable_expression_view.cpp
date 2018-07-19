#include "scrollable_expression_view.h"
#include <assert.h>
using namespace Poincare;

namespace Calculation {

ScrollableExpressionView::ScrollableExpressionView(Responder * parentResponder) :
  ScrollableView(parentResponder, &m_expressionView, this),
  m_expressionView()
{
}

void ScrollableExpressionView::setLayoutRef(LayoutRef layoutRef) {
  m_expressionView.setLayoutRef(layoutRef);
  layoutSubviews();
}

void ScrollableExpressionView::setBackgroundColor(KDColor backgroundColor) {
  m_expressionView.setBackgroundColor(backgroundColor);
  ScrollableView::setBackgroundColor(backgroundColor);
}

KDSize ScrollableExpressionView::minimalSizeForOptimalDisplay() const {
  return m_expressionView.minimalSizeForOptimalDisplay();
}

}
