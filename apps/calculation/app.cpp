#include "app.h"
#include "../apps_container.h"
#include "calculation_icon.h"
#include "../i18n.h"

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
  return new App(container, this);
}

void App::Snapshot::reset() {
  m_calculationStore.deleteAll();
}

App::Descriptor * App::Snapshot::descriptor() {
  static Descriptor descriptor;
  return &descriptor;
}

CalculationStore * App::Snapshot::calculationStore() {
  return &m_calculationStore;
}

void App::Snapshot::tidy() {
  m_calculationStore.tidy();
}

App::App(Container * container, Snapshot * snapshot) :
  TextFieldDelegateApp(container, snapshot, &m_editExpressionController),
  m_localContext((GlobalContext *)((AppsContainer *)container)->globalContext(), snapshot->calculationStore()),
  m_historyController(&m_editExpressionController, snapshot->calculationStore()),
  m_editExpressionController(&m_modalViewController, &m_historyController, snapshot->calculationStore())
{
}

Context * App::localContext() {
  return &m_localContext;
}

bool App::textFieldDidReceiveEvent(::TextField * textField, Ion::Events::Event event) {
  if (TextFieldDelegateApp::textFieldDidReceiveEvent(textField, event)) {
    return true;
  }
  /* Here, we check that the expression entered by the user can be printed with
   * less than k_printedExpressionLength characters. Otherwise, we prevent the
   * user from adding this expression to the calculation store. */
  if (textField->isEditing() && textField->textFieldShouldFinishEditing(event)) {
    Expression * exp = Expression::parse(textField->text());
    assert(exp != nullptr);
    char buffer[Calculation::k_printedExpressionSize];
    int length = exp->writeTextInBuffer(buffer, sizeof(buffer));
    delete exp;
    /* if the buffer is totally full, it is VERY likely that writeTextInBuffer
     * escaped before printing utterly the expression. */
    if (length >= Calculation::k_printedExpressionSize-1) {
      displayWarning(I18n::Message::SyntaxError);
      return true;
    }
  }
  return false;
}

const char * App::XNT() {
  return "x";
}

}
