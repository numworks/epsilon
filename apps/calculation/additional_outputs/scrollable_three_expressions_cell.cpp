#include "scrollable_three_expressions_cell.h"
#include <poincare/exception_checkpoint.h>
#include "../app.h"

namespace Calculation {

void ScrollableThreeExpressionsView::resetMemoization() {
  setLayouts(Poincare::Layout(), Poincare::Layout(), Poincare::Layout());
}

void ScrollableThreeExpressionsView::setCalculation(Calculation * calculation) {
  Poincare::Context * context = App::app()->localContext();

  // Clean the layouts to make room in the pool
  resetMemoization();

  // Create the input layout
  Poincare::Layout inputLayout = calculation->createInputLayout();

  // Create the exact output layout
  Poincare::Layout exactOutputLayout = Poincare::Layout();
  if (Calculation::DisplaysExact(calculation->displayOutput(context))) {
    bool couldNotCreateExactLayout = false;
    exactOutputLayout = calculation->createExactOutputLayout(&couldNotCreateExactLayout);
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
  Poincare::Layout approximateOutputLayout = Poincare::Layout();
  if (displayOutput == Calculation::DisplayOutput::ExactOnly) {
    approximateOutputLayout = exactOutputLayout;
  } else {
    bool couldNotCreateApproximateLayout = false;
    approximateOutputLayout = calculation->createApproximateOutputLayout(context, &couldNotCreateApproximateLayout);
    if (couldNotCreateApproximateLayout) {
      if (calculation->displayOutput(context) == ::Calculation::Calculation::DisplayOutput::ApproximateOnly) {
        Poincare::ExceptionCheckpoint::Raise();
      } else {
        /* Set the display output to ApproximateOnly, make room in the pool by
         * erasing the exact layout, and retry to create the approximate layout */
        calculation->forceDisplayOutput(::Calculation::Calculation::DisplayOutput::ApproximateOnly);
        exactOutputLayout = Poincare::Layout();
        couldNotCreateApproximateLayout = false;
        approximateOutputLayout = calculation->createApproximateOutputLayout(context, &couldNotCreateApproximateLayout);
        if (couldNotCreateApproximateLayout) {
          Poincare::ExceptionCheckpoint::Raise();
        }
      }
    }

  }
  setLayouts(inputLayout, exactOutputLayout, approximateOutputLayout);
  I18n::Message equalMessage = calculation->exactAndApproximateDisplayedOutputsAreEqual(context) == Calculation::EqualSign::Equal ? I18n::Message::Equal : I18n::Message::AlmostEqual;
  setEqualMessage(equalMessage);

  /* The displayed input and outputs have changed. We need to re-layout the cell
   * and re-initialize the scroll. */
  layoutSubviews();
}

KDCoordinate ScrollableThreeExpressionsCell::Height(Calculation * calculation) {
  ScrollableThreeExpressionsCell cell;
  cell.setCalculation(calculation);
  KDRect leftFrame = KDRectZero;
  KDRect centerFrame = KDRectZero;
  KDRect approximateSignFrame = KDRectZero;
  KDRect rightFrame = KDRectZero;
  cell.subviewFrames(&leftFrame, &centerFrame, &approximateSignFrame, &rightFrame);
  KDRect unionedFrame = leftFrame.unionedWith(centerFrame).unionedWith(rightFrame);
  return unionedFrame.height() + 2 * ScrollableThreeExpressionsView::k_margin;
}

void ScrollableThreeExpressionsCell::didBecomeFirstResponder() {
  reinitSelection();
  Container::activeApp()->setFirstResponder(&m_view);
}

void ScrollableThreeExpressionsCell::reinitSelection() {
  m_view.setSelectedSubviewPosition(ScrollableThreeExpressionsView::SubviewPosition::Left);
  m_view.reloadScroll();
}

void ScrollableThreeExpressionsCell::setCalculation(Calculation * calculation) {
  m_view.setCalculation(calculation);
  layoutSubviews();
}

void ScrollableThreeExpressionsCell::setDisplayCenter(bool display) {
  m_view.setDisplayCenter(display);
  layoutSubviews();
}

}
