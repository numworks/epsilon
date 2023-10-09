#include "main_controller.h"

#include <apps/i18n.h>
#include <apps/shared/poincare_helpers.h>
#include <assert.h>
#include <escher/container.h>
#include <poincare_junior/include/expression.h>
#include <poincare_junior/include/layout.h>

#include "app.h"

using namespace CalculationJunior;

MainController::MainController(Escher::StackViewController* parentResponder)
    : Escher::ViewController(parentResponder), m_view(this) {}

void MainController::didBecomeFirstResponder() {
  App::app()->setFirstResponder(m_view.layoutField());
}

bool MainController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK || event == Ion::Events::EXE) {
    m_view.reductionLayoutView()->setLayout(PoincareJ::Layout());
    PoincareJ::Layout inputLayout = m_view.layoutField()->layout();
    PoincareJ::Expression inputExpression =
        PoincareJ::Expression::Parse(&inputLayout);
    if (inputExpression.isUninitialized()) {
      return false;
    }
    PoincareJ::Expression reducedExpression =
        PoincareJ::Expression::Simplify(&inputExpression);
    PoincareJ::Layout layout =
        PoincareJ::Layout::FromExpression(&reducedExpression);
    m_view.reductionLayoutView()->setLayout(layout);
    // Approximate reduced expression
    float approximation = reducedExpression.approximate<float>();
    constexpr int bufferSize = 220;
    char buffer[bufferSize];
    Shared::PoincareHelpers::ConvertFloatToText<float>(
        approximation, buffer, bufferSize,
        Poincare::Preferences::VeryLargeNumberOfSignificantDigits);
    m_view.approximationTextView()->setText(buffer);
    m_view.layoutSubviews();
    return true;
  }
  return false;
}

// ContentView

MainController::ContentView::ContentView(Escher::Responder* parentResponder)
    : View(),
      m_layoutField(parentResponder),
      m_reductionLayoutView(),
      m_approximationView(
          KDGlyph::Format{.horizontalAlignment = KDGlyph::k_alignRight}) {}

Escher::View* MainController::ContentView::subviewAtIndex(int index) {
  Escher::View* subviews[k_numberOfSubviews] = {
      &m_layoutField, &m_reductionLayoutView, &m_approximationView};
  return subviews[index];
}

void MainController::ContentView::layoutSubviews(bool force) {
  KDCoordinate width = bounds().width();
  KDCoordinate height = bounds().height();
  KDCoordinate reductionHeight =
      m_reductionLayoutView.minimalSizeForOptimalDisplay().height();
  KDCoordinate approximationHeight =
      m_approximationView.minimalSizeForOptimalDisplay().height();
  KDCoordinate layoutHeight = height - reductionHeight - approximationHeight;

  setChildFrame(&m_layoutField, KDRect(0, 0, width, layoutHeight), force);
  setChildFrame(&m_reductionLayoutView,
                KDRect(0, layoutHeight, width, reductionHeight), force);
  setChildFrame(
      &m_approximationView,
      KDRect(0, layoutHeight + reductionHeight, width, approximationHeight),
      force);
}
