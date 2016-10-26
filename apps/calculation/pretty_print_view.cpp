#include "pretty_print_view.h"
#include <assert.h>

namespace Calculation {

PrettyPrintView::PrettyPrintView(Responder * parentResponder) :
  ScrollView(&m_expressionView),
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
  KDSize expressionSize = m_expressionView.minimalSizeForOptimalDisplay();
  if (expressionSize.width() >= bounds().width()) {
    m_expressionView.setSize(expressionSize);
  } else {
    m_expressionView.setSize(KDSize(bounds().width(), expressionSize.height()));
  }
  ScrollView::layoutSubviews();
}

KDSize PrettyPrintView::minimalSizeForOptimalDisplay() {
  return m_expressionView.minimalSizeForOptimalDisplay();
}

bool PrettyPrintView::handleEvent(Ion::Events::Event event) {
  switch (event) {
    case Ion::Events::Event::RIGHT_ARROW:
      m_manualScrolling += 10;
      setContentOffset({m_manualScrolling, 0});
      return true;
    case Ion::Events::Event::LEFT_ARROW:
      m_manualScrolling -= 10;
      setContentOffset({m_manualScrolling, 0});
      return true;
    default:
      return false;
  }
}

}
