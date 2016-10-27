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
  setContentOffset({0, 0});
}

bool PrettyPrintView::rightViewIsInvisible() {
  return m_expressionView.bounds().width() - m_manualScrolling > bounds().width();
}

bool PrettyPrintView::handleEvent(Ion::Events::Event event) {
  switch (event) {
    case Ion::Events::Event::RIGHT_ARROW:
      if (rightViewIsInvisible()) {
        m_manualScrolling += 10;
        setContentOffset({m_manualScrolling, 0});
        return true;
      }
      return false;
    case Ion::Events::Event::LEFT_ARROW:
      if (m_manualScrolling > 0) {
        m_manualScrolling -= 10;
        setContentOffset({m_manualScrolling, 0});
        return true;
      }
      return false;
    default:
      return false;
  }
}

}
