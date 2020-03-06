#include "app.h"
#include "calculation_icon.h"
#include <apps/i18n.h>
#include <poincare/symbol.h>

using namespace Poincare;

using namespace Shared;

namespace Calculation {

I18n::Message App::Descriptor::name() {
  return I18n::Message::CalculApp;
}

I18n::Message App::Descriptor::upperName() {
  return I18n::Message::CalculAppCapital;
}

const Image * App::Descriptor::icon() {
  return ImageStore::CalculationIcon;
}

App * App::Snapshot::unpack(Container * container) {
  return new (container->currentAppBuffer()) App(this);
}

void App::Snapshot::reset() {
  m_calculationStore.deleteAll();
}

App::Descriptor * App::Snapshot::descriptor() {
  static Descriptor descriptor;
  return &descriptor;
}

void App::Snapshot::tidy() {
  m_calculationStore.tidy();
}

App::App(Snapshot * snapshot) :
  ExpressionFieldDelegateApp(snapshot, &m_editExpressionController),
  m_historyController(&m_editExpressionController, snapshot->calculationStore()),
  m_editExpressionController(&m_modalViewController, this, &m_historyController, snapshot->calculationStore())
{
}

bool App::textFieldDidReceiveEvent(::TextField * textField, Ion::Events::Event event) {
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
  {
    Expression ansExpression = static_cast<Snapshot *>(snapshot())->calculationStore()->ansExpression(localContext());
    if (!TextFieldDelegateApp::ExpressionCanBeSerialized(expression, true, ansExpression, localContext())) {
      return false;
    }
  }
  return !expression.isUninitialized();
}

}
