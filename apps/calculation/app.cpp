#include "app.h"
#include "../apps_container.h"
#include "../shared/poincare_helpers.h"
#include "calculation_icon.h"
#include "../i18n.h"
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
  return new (container->currentAppBuffer()) App(container, this);
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

App::App(Container * container, Snapshot * snapshot) :
  ExpressionFieldDelegateApp(container, snapshot, &m_editExpressionController),
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

bool App::isAcceptableExpression(const Poincare::Expression expression, Responder * responder) {
  /* Here, we check that the expression entered by the user can be printed with
   * less than k_printedExpressionLength characters. Otherwise, we prevent the
   * user from adding this expression to the calculation store. */
  Expression exp = expression;
  if (exp.isUninitialized()) {
    return false;
  }
  Expression ansExpression = static_cast<Snapshot *>(snapshot())->calculationStore()->ansExpression(localContext());
  {
    Symbol ansSymbol = Symbol::Ans();
    exp = exp.replaceSymbolWithExpression(ansSymbol, ansExpression);
  }
  char buffer[Calculation::k_printedExpressionSize];
  int length = PoincareHelpers::Serialize(exp, buffer, sizeof(buffer));
  /* if the buffer is totally full, it is VERY likely that writeTextInBuffer
   * escaped before printing utterly the expression. */
  if (length >= Calculation::k_printedExpressionSize-1) {
    return false;
  }
  exp = Expression::parse(buffer);
  if (exp.isUninitialized()) {
    // The ans replacement made the expression unparsable
    return false;
  }
  return true;
}

char App::XNT() {
  return 'x';
}

}
