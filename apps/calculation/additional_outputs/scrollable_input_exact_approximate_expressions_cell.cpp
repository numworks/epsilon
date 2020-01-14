#include "scrollable_input_exact_approximate_expressions_cell.h"
#include <poincare/exception_checkpoint.h>
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

  // Create the input layout
  contentCell()->leftExpressionView()->setLayout(calculation->createInputLayout());

  // Create the exact output layout
  Poincare::Layout leftOutputLayout = Poincare::Layout();
  if (Calculation::DisplaysExact(calculation->displayOutput(context))) {
    bool couldNotCreateExactLayout = false;
    leftOutputLayout = calculation->createExactOutputLayout(&couldNotCreateExactLayout);
    if (couldNotCreateExactLayout) {
      if (calculation->displayOutput(context) == ::Calculation::Calculation::DisplayOutput::ExactOnly) {
        Poincare::ExceptionCheckpoint::Raise();
      } else {
        calculation->forceDisplayOutput(::Calculation::Calculation::DisplayOutput::ApproximateOnly);
      }
    }
  }
  Calculation::DisplayOutput displayOutput = calculation->displayOutput(context);

  // Create the approximate output layout
  Poincare::Layout rightOutputLayout = Poincare::Layout();
  if (displayOutput == Calculation::DisplayOutput::ExactOnly) {
    rightOutputLayout = leftOutputLayout;
  } else {
    bool couldNotCreateApproximateLayout = false;
    rightOutputLayout = calculation->createApproximateOutputLayout(context, &couldNotCreateApproximateLayout);
    if (couldNotCreateApproximateLayout) {
      if (calculation->displayOutput(context) == ::Calculation::Calculation::DisplayOutput::ApproximateOnly) {
        Poincare::ExceptionCheckpoint::Raise();
      } else {
        /* Set the display output to ApproximateOnly, make room in the pool by
         * erasing the exact layout, and retry to create the approximate layout */
        calculation->forceDisplayOutput(::Calculation::Calculation::DisplayOutput::ApproximateOnly);
        leftOutputLayout = Poincare::Layout();
        couldNotCreateApproximateLayout = false;
        rightOutputLayout = calculation->createApproximateOutputLayout(context, &couldNotCreateApproximateLayout);
        if (couldNotCreateApproximateLayout) {
          Poincare::ExceptionCheckpoint::Raise();
        }
      }
    }

  }
  setLayouts(rightOutputLayout, leftOutputLayout);
  I18n::Message equalMessage = calculation->exactAndApproximateDisplayedOutputsAreEqual(context) == Calculation::EqualSign::Equal ? I18n::Message::Equal : I18n::Message::AlmostEqual;
  setEqualMessage(equalMessage);

  /* The displayed input and outputs have changed. We need to re-layout the cell
   * and re-initialize the scroll. */
  layoutSubviews();
  reloadScroll();
}

void ScrollableInputExactApproximateExpressionsCell::didBecomeFirstResponder() {
  m_view.setSelectedSubviewPosition(Shared::ScrollableExactApproximateExpressionsView::SubviewPosition::Left);
  Container::activeApp()->setFirstResponder(&m_view);
}

void ScrollableInputExactApproximateExpressionsCell::setCalculation(Calculation * calculation) {
  m_view.setCalculation(calculation);
  layoutSubviews();
}

void ScrollableInputExactApproximateExpressionsCell::setDisplayCenter(bool display) {
  m_view.setDisplayCenter(display);
  layoutSubviews();
}

void ScrollableInputExactApproximateExpressionsCell::setDisplayLeft(bool display) {
  m_view.setDisplayLeft(display);
  layoutSubviews();
}

}
