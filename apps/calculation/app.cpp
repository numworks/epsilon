#include "app.h"
#include "calculation_icon.h"
#include <apps/i18n.h>
#include <poincare/comparison_operator.h>
#include <poincare/symbol.h>

using namespace Poincare;
using namespace Shared;
using namespace Escher;

namespace Calculation {

I18n::Message App::Descriptor::name() const {
  return I18n::Message::CalculApp;
}

I18n::Message App::Descriptor::upperName() const {
  return I18n::Message::CalculAppCapital;
}

const Image * App::Descriptor::icon() const {
  return ImageStore::CalculationIcon;
}

App * App::Snapshot::unpack(Container * container) {
  return new (container->currentAppBuffer()) App(this);
}

void App::Snapshot::reset() {
  m_calculationStore.deleteAll();
  m_cacheBuffer[0] = 0;
  m_cacheBufferInformation = 0;
}

constexpr static App::Descriptor sDescriptor;

const App::Descriptor * App::Snapshot::descriptor() const {
  return &sDescriptor;
}

App::Snapshot::Snapshot() : m_calculationStore(m_calculationBuffer, k_calculationBufferSize)
{
}

App::App(Snapshot * snapshot) :
  ExpressionFieldDelegateApp(snapshot, &m_editExpressionController),
  m_historyController(&m_editExpressionController, snapshot->calculationStore()),
  m_editExpressionController(&m_modalViewController, this, snapshot->cacheBuffer(), snapshot->cacheBufferInformationAddress(), &m_historyController, snapshot->calculationStore())
{
}

bool App::textFieldDidReceiveEvent(AbstractTextField * textField, Ion::Events::Event event) {
  if (textField->isEditing() && textField->shouldFinishEditing(event) && textField->text()[0] == 0) {
    return true;
  }
  return Shared::ExpressionFieldDelegateApp::textFieldDidReceiveEvent(textField, event);
}

bool App::layoutFieldDidReceiveEvent(::LayoutField * layoutField, Ion::Events::Event event) {
  if (layoutField->isEditing() && layoutField->shouldFinishEditing(event) && !layoutField->hasText()) {
    return true;
  }
  return Shared::ExpressionFieldDelegateApp::layoutFieldDidReceiveEvent(layoutField, event);
}

bool App::isAcceptableExpression(const Poincare::Expression expression) {
  /* Override ExpressionFieldDelegateApp because Store is acceptable, and
   * ans has an expression. */
  {
    Expression ansExpression = static_cast<Snapshot *>(snapshot())->calculationStore()->ansExpression(localContext());
    if (!TextFieldDelegateApp::ExpressionCanBeSerialized(expression, true, ansExpression, localContext())) {
      return false;
    }
  }
  return !(expression.isUninitialized() || ComparisonOperator::IsComparisonOperatorType(expression.type()));
}

void App::didBecomeActive(Window * window) {
  m_editExpressionController.restoreInput();
  m_historyController.recomputeHistoryCellHeightsIfNeeded();
  Shared::ExpressionFieldDelegateApp::didBecomeActive(window);
}

void App::willBecomeInactive() {
  m_editExpressionController.memoizeInput();
  Shared::ExpressionFieldDelegateApp::willBecomeInactive();
}

}
