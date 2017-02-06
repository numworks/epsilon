#include "scrollable_expression_view.h"
#include <assert.h>
using namespace Poincare;

namespace Calculation {

ScrollableExpressionView::ScrollableExpressionView(Responder * parentResponder) :
  ScrollView(&m_expressionView, 0, 0, 0, 0, false),
  Responder(parentResponder),
  m_expressionView(ExpressionView()),
  m_manualScrolling(0)
{
}

void ScrollableExpressionView::setExpression(ExpressionLayout * expressionLayout) {
  m_expressionView.setExpression(expressionLayout);
  layoutSubviews();
}

void ScrollableExpressionView::setBackgroundColor(KDColor backgroundColor) {
  m_expressionView.setBackgroundColor(backgroundColor);
}

void ScrollableExpressionView::layoutSubviews() {
  m_expressionView.setSize(m_expressionView.minimalSizeForOptimalDisplay());
  ScrollView::layoutSubviews();
}

KDSize ScrollableExpressionView::minimalSizeForOptimalDisplay() {
  return m_expressionView.minimalSizeForOptimalDisplay();
}

void ScrollableExpressionView::reloadCell() {
  m_manualScrolling = 0;
  setContentOffset(KDPoint(m_manualScrolling, 0));
}

bool ScrollableExpressionView::rightViewIsInvisible() {
  return m_expressionView.bounds().width() - m_manualScrolling > bounds().width();
}

bool ScrollableExpressionView::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Right && rightViewIsInvisible()) {
      KDCoordinate rightSpace = m_expressionView.bounds().width() - m_manualScrolling - bounds().width();
      KDCoordinate scrollAdd = rightSpace > 10 ? 10 : rightSpace;
      m_manualScrolling += scrollAdd;
      setContentOffset(KDPoint(m_manualScrolling, 0));
      return true;
  }
  if (event == Ion::Events::Left && m_manualScrolling > 0) {
    KDCoordinate leftSpace = m_manualScrolling;
    KDCoordinate scrollSubstract = leftSpace > 10 ? 10 : leftSpace;
    m_manualScrolling -= scrollSubstract;
    setContentOffset(KDPoint(m_manualScrolling, 0));
    return true;
  }
  return false;
}

}
