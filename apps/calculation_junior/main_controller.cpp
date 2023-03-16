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
    Shared::TextFieldDelegateApp* textFieldDelegateApp)
    : Escher::ViewController(parentResponder),
      m_view(this, textFieldDelegateApp) {}

void MainController::didBecomeFirstResponder() {
  Escher::Container::activeApp()->setFirstResponder(m_view.textField());
}

bool MainController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK || event == Ion::Events::EXE) {
    // Parse input text into Layout. TODO : Input a Layout directly
    m_view.inputLayoutView()->setLayout(
        PoincareJ::Layout::Parse(m_view.textField()->text()));
    /* Dump parsed expression into m_buffer.blocks() TypeBlock array.
     * TODO : CreateBasicReduction from expressions. */
    PoincareJ::Layout tempLayout = m_view.inputLayoutView()->getLayout();
    PoincareJ::Expression::Parse(&tempLayout).dumpAt(m_buffer.blocks());
    // The reduced expression has to live somewhere so layout can be initialized
    m_reducedExpression =
        PoincareJ::Expression::CreateBasicReduction(m_buffer.blocks());
    m_view.reductionLayoutView()->setLayout(m_reducedExpression.toLayout());
    // Approximate reduced expression
    float approximation = m_reducedExpression.approximate();
    constexpr int bufferSize = 220;
    char buffer[bufferSize];
    Shared::PoincareHelpers::ConvertFloatToText<float>(
        approximation, buffer, bufferSize,
        Poincare::Preferences::VeryLargeNumberOfSignificantDigits);
    m_view.approximationTextView()->setText(buffer);
  }
  return false;
}

// ContentView

MainController::ContentView::ContentView(
    Escher::Responder* parentResponder,
    Shared::TextFieldDelegateApp* textFieldDelegateApp)
    : View(),
      m_buffer(""),
      m_textField(parentResponder, m_buffer, k_bufferSize, textFieldDelegateApp,
                  textFieldDelegateApp),
      m_inputLayoutView(),
      m_reductionLayoutView(),
      m_approximationView(
          KDGlyph::Format{.horizontalAlignment = KDGlyph::k_alignRight}) {}

Escher::View* MainController::ContentView::subviewAtIndex(int index) {
  Escher::View* subviews[k_numberOfSubviews] = {
      &m_textField, &m_inputLayoutView, &m_reductionLayoutView,
      &m_approximationView};
  return subviews[index];
}

void MainController::ContentView::layoutSubviews(bool force) {
  KDCoordinate width = bounds().width();
  KDCoordinate y = 0;
  setChildFrame(&m_textField, KDRect(0, y, width, k_textViewHeight), force);
  y += k_textViewHeight;
  setChildFrame(&m_inputLayoutView, KDRect(0, y, width, k_LayoutViewHeight),
                force);
  y += k_LayoutViewHeight;
  setChildFrame(&m_reductionLayoutView, KDRect(0, y, width, k_LayoutViewHeight),
                force);
  y += k_LayoutViewHeight;
  setChildFrame(&m_approximationView, KDRect(0, y, width, k_textViewHeight),
                force);
  assert(bounds().height() == y + k_textViewHeight);
}
