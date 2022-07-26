#include "scrollable_three_expressions_cell.h"
#include <poincare/exception_checkpoint.h>
#include "../app.h"

using namespace Escher;

namespace Calculation {

void ScrollableThreeExpressionsView::resetMemoization() {
  setLayouts(Poincare::Layout(), Poincare::Layout(), Poincare::Layout());
}

// TODO: factorize with HistoryViewCell!
void ScrollableThreeExpressionsView::setCalculation(Calculation * calculation, Poincare::Context * context, bool canChangeDisplayOutput) {
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
      if (canChangeDisplayOutput && calculation->displayOutput(context) != ::Calculation::Calculation::DisplayOutput::ExactOnly) {
        calculation->forceDisplayOutput(::Calculation::Calculation::DisplayOutput::ApproximateOnly);
      } else {
        Poincare::ExceptionCheckpoint::Raise();
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
    approximateOutputLayout = calculation->createApproximateOutputLayout(&couldNotCreateApproximateLayout);
    if (couldNotCreateApproximateLayout) {
      if (canChangeDisplayOutput && calculation->displayOutput(context) != ::Calculation::Calculation::DisplayOutput::ApproximateOnly) {
        /* Set the display output to ApproximateOnly, make room in the pool by
         * erasing the exact layout, and retry to create the approximate layout */
        calculation->forceDisplayOutput(::Calculation::Calculation::DisplayOutput::ApproximateOnly);
        exactOutputLayout = Poincare::Layout();
        couldNotCreateApproximateLayout = false;
        approximateOutputLayout = calculation->createApproximateOutputLayout(&couldNotCreateApproximateLayout);
        if (couldNotCreateApproximateLayout) {
          Poincare::ExceptionCheckpoint::Raise();
        }
      } else {
        Poincare::ExceptionCheckpoint::Raise();
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

KDCoordinate ScrollableThreeExpressionsCell::Height(Calculation * calculation, Poincare::Context * context) {
  ScrollableThreeExpressionsCell cell;
  cell.setCalculation(calculation, context, true);
  KDRect leftFrame = KDRectZero;
  KDRect centerFrame = KDRectZero;
  KDRect approximateSignFrame = KDRectZero;
  KDRect rightFrame = KDRectZero;
  cell.subviewFrames(&leftFrame, &centerFrame, &approximateSignFrame, &rightFrame);
  KDRect unionedFrame = leftFrame.unionedWith(centerFrame).unionedWith(rightFrame);
  return unionedFrame.height();
}

void ScrollableThreeExpressionsCell::setLayouts(Poincare::Layout leftLayout, Poincare::Layout centerLayout, Poincare::Layout rightLayout) {
  m_view.setLayouts(leftLayout, centerLayout, rightLayout);
  m_view.setShowEqual(!centerLayout.isUninitialized() || !rightLayout.isUninitialized());
}

void ScrollableThreeExpressionsCell::didBecomeFirstResponder() {
  reinitSelection();
  Container::activeApp()->setFirstResponder(&m_view);
}

void ScrollableThreeExpressionsCell::reinitSelection() {
  m_view.setSelectedSubviewPosition(m_view.leftMostPosition());
  m_view.reloadScroll();
}

void ScrollableThreeExpressionsCell::setCalculation(Calculation * calculation, Poincare::Context * context, bool canChangeDisplayOutput) {
  m_view.setCalculation(calculation, context, canChangeDisplayOutput);
  layoutSubviews();
}

void ScrollableThreeExpressionsCell::setDisplayCenter(bool display) {
  m_view.setDisplayCenter(display);
  layoutSubviews();
}

}
