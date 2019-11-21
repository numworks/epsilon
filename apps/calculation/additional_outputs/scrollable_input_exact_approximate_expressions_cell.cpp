#include "scrollable_input_exact_approximate_expressions_cell.h"
#include "../app.h"

namespace Calculation {

Poincare::Layout ScrollableInputExactApproximateExpressionsView::ContentCell::layout() const {
  if (selectedSubviewPosition() == SubviewPosition::Left) {
    return m_leftExpressionView.layout();
  }
  return AbstractScrollableExactApproximateExpressionsView::ContentCell::layout();
}

void ScrollableInputExactApproximateExpressionsView::setCalculation(Calculation * calculation) {
  Poincare::Context * context = App::app()->localContext();

  // Clean the layouts to make room in the pool
  contentCell()->leftExpressionView()->setLayout(Poincare::Layout());
  setLayouts(Poincare::Layout(), Poincare::Layout());

  Calculation::DisplayOutput displayOutput = calculation->displayOutput(context);
  contentCell()->leftExpressionView()->setLayout(calculation->createInputLayout());
  Poincare::Layout leftOutputLayout = calculation->createExactOutputLayout();
  Poincare::Layout rightOutputLayout = (displayOutput == Calculation::DisplayOutput::ExactOnly) ? leftOutputLayout :
    calculation->createApproximateOutputLayout(context);
  setLayouts(rightOutputLayout, leftOutputLayout);
  I18n::Message equalMessage = calculation->exactAndApproximateDisplayedOutputsAreEqual(context) == Calculation::EqualSign::Equal ? I18n::Message::Equal : I18n::Message::AlmostEqual;
  setEqualMessage(equalMessage);

  /* The displayed input and outputs have changed. We need to re-layout the cell
   * and re-initialize the scroll. */
  layoutSubviews();
  reloadScroll();
}

void ScrollableInputExactApproximateExpressionsCell::didBecomeFirstResponder() {
  Container::activeApp()->setFirstResponder(&m_view);
}

}
