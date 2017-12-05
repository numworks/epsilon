#include "controller.h"

namespace ExpressionEditor {

Controller::ContentView::ContentView(ExpressionAndLayout * expressionAndLayout) :
  SolidColorView(KDColorWhite),
  m_expressionView()
{
  m_expressionView.setExpressionLayout(expressionAndLayout->expressionLayout());
}

void Controller::ContentView::layoutSubviews() {
  m_expressionView.setFrame(KDRect(
    k_margin,
    k_margin,
    bounds().width() - 2 * k_margin,
    bounds().height() - 2 * k_margin));
}

KDSize Controller::ContentView::minimalSizeForOptimalDisplay() const {
  return m_expressionView.minimalSizeForOptimalDisplay();
}

Controller::Controller(Responder * parentResponder, ExpressionAndLayout * expressionAndLayout) :
  ViewController(parentResponder),
  m_view(expressionAndLayout)
{
}

bool Controller::handleEvent(Ion::Events::Event event) {
  return false;
}

}
