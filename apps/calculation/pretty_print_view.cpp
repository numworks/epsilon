#include "pretty_print_view.h"
#include <assert.h>

namespace Calculation {

PrettyPrintView::PrettyPrintView(Responder * parentResponder) :
  ScrollView(&m_expressionView, 0, 0, 0, 0, false),
  Responder(parentResponder),
  m_expressionView(ExpressionView()),
  m_manualScrolling(0)
{
}

void PrettyPrintView::setExpression(ExpressionLayout * expressionLayout) {
  m_expressionView.setExpression(expressionLayout);
  layoutSubviews();
}

void PrettyPrintView::setBackgroundColor(KDColor backgroundColor) {
  m_expressionView.setBackgroundColor(backgroundColor);
}

void PrettyPrintView::layoutSubviews() {
  m_expressionView.setSize(m_expressionView.minimalSizeForOptimalDisplay());
  ScrollView::layoutSubviews();
}

KDSize PrettyPrintView::minimalSizeForOptimalDisplay() {
  return m_expressionView.minimalSizeForOptimalDisplay();
}

void PrettyPrintView::reloadCell() {
  m_manualScrolling = 0;
  setContentOffset(KDPoint(m_manualScrolling, 0));
}

bool PrettyPrintView::rightViewIsInvisible() {
  return m_expressionView.bounds().width() - m_manualScrolling > bounds().width();
}

bool PrettyPrintView::handleEvent(Ion::Events::Event event) {
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
