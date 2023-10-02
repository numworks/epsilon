#include "main_controller.h"

#include <apps/i18n.h>
#include <apps/shared/poincare_helpers.h>
#include <assert.h>
#include <escher/container.h>
#include <poincare_junior/include/expression.h>
#include <poincare_junior/include/layout.h>

#include "app.h"

using namespace CalculationJunior;

MainController::MainController(
    Escher::StackViewController* parentResponder,
    ExpressionFieldDelegateApp* expressionFieldDelegateApp)
    : Escher::ViewController(parentResponder),
      m_view(this, expressionFieldDelegateApp) {}

void MainController::didBecomeFirstResponder() {
  Escher::Container::activeApp()->setFirstResponder(m_view.layoutField());
}

bool MainController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK || event == Ion::Events::EXE) {
    // Reset reduction layout before altering m_buffer
    m_view.reductionLayoutView()->setLayout(PoincareJ::Layout());
    /* Create temporary Layout and expression to dump into m_buffer.blocks().
     * TODO : CreateSimplifyReduction from expressions. */
    PoincareJ::Layout tempLayout = m_view.layoutField()->layout();
    PoincareJ::Expression tempExpression =
        PoincareJ::Expression::Parse(&tempLayout);
    if (tempExpression.isUninitialized()) {
      return false;
    }
    tempExpression.dumpAt(m_buffer.blocks());
    // The reduced expression has to live somewhere so layout can be initialized
    m_reducedExpression =
        PoincareJ::Expression::CreateSimplifyReduction(m_buffer.blocks());
    m_view.reductionLayoutView()->setLayout(m_reducedExpression.toLayout());
    // Approximate reduced expression
    float approximation = m_reducedExpression.approximate<float>();
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

MainController::ContentView::ContentView(
    Escher::Responder* parentResponder,
    ExpressionFieldDelegateApp* expressionFieldDelegateApp)
    : View(),
      m_buffer(""),
      m_layoutField(parentResponder, expressionFieldDelegateApp),
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
