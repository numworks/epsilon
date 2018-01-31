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
  m_historyController(&m_editExpressionController, snapshot->calculationStore()),
  m_editExpressionController(&m_modalViewController, &m_historyController, snapshot->calculationStore())
{
}

bool App::textFieldDidReceiveEvent(::TextField * textField, Ion::Events::Event event) {
  if ((event == Ion::Events::Var ||  event == Ion::Events::XNT) && TextFieldDelegateApp::textFieldDidReceiveEvent(textField, event)) {
    return true;
  }
  if (textField->isEditing() && textField->textFieldShouldFinishEditing(event)) {
    if (textField->text()[0] == 0) {
      return true;
    }
    if (!textInputIsCorrect(textField->text())) {
      displayWarning(I18n::Message::SyntaxError);
      return true;
    }
  }
  return false;
}

bool App::textInputIsCorrect(const char * text) {
  /* Here, we check that the expression entered by the user can be printed with
   * less than k_printedExpressionLength characters. Otherwise, we prevent the
   * user from adding this expression to the calculation store. */
  Expression * exp = Expression::parse(text);
  if (exp == nullptr) {
    return false;
  }
  Expression::ReplaceSymbolWithExpression(&exp, Symbol::SpecialSymbols::Ans, static_cast<Snapshot *>(snapshot())->calculationStore()->ansExpression(localContext()));
  char buffer[Calculation::k_printedExpressionSize];
  int length = exp->writeTextInBuffer(buffer, sizeof(buffer));
  delete exp;
  /* if the buffer is totally full, it is VERY likely that writeTextInBuffer
   * escaped before printing utterly the expression. */
  if (length >= Calculation::k_printedExpressionSize-1) {
    return false;
  }
  return true;
}

const char * App::XNT() {
  return "x";
}

}
