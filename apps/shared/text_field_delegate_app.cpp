#include "text_field_delegate_app.h"
#include "../apps_container.h"
#include <cmath>
#include <string.h>

using namespace Poincare;

namespace Shared {

TextFieldDelegateApp::TextFieldDelegateApp(Container * container, Snapshot * snapshot, ViewController * rootViewController) :
  InputEventHandlerDelegateApp(container, snapshot, rootViewController),
  TextFieldDelegate()
{
}

Context * TextFieldDelegateApp::localContext() {
  return container()->globalContext();
}

char TextFieldDelegateApp::XNT() {
  return 'X';
}

bool TextFieldDelegateApp::textFieldShouldFinishEditing(TextField * textField, Ion::Events::Event event) {
  return isFinishingEvent(event);
}

bool TextFieldDelegateApp::textFieldDidReceiveEvent(TextField * textField, Ion::Events::Event event) {
  if (textField->isEditing() && textField->shouldFinishEditing(event)) {
    if (unparsableText(textField->text(), textField)) {
      return true;
    }
  }
  if (fieldDidReceiveEvent(textField, textField, event)) {
    return true;
  }
  return false;
}

/* Protected */

bool TextFieldDelegateApp::fieldDidReceiveEvent(EditableField * field, Responder * responder, Ion::Events::Event event) {
  if (event == Ion::Events::Var) {
    forceEdition(field);
    AppsContainer * appsContainer = (AppsContainer *)responder->app()->container();
    VariableBoxController * variableBoxController = appsContainer->variableBoxController();
    variableBoxController->setSender(field);
    responder->app()->displayModalViewController(variableBoxController, 0.f, 0.f, Metric::PopUpTopMargin, Metric::PopUpLeftMargin, 0, Metric::PopUpRightMargin);
    return true;

  }
  if (event == Ion::Events::XNT) {
    forceEdition(field);
    const char xnt[2] = {field->XNTChar(XNT()), 0};
    return field->handleEventWithText(xnt);
  }
  return false;
}

void TextFieldDelegateApp::forceEdition(EditableField * field) {
  if (!field->isEditing()) {
    field->setEditing(true);
  }
}

bool TextFieldDelegateApp::isFinishingEvent(Ion::Events::Event event) {
  return event == Ion::Events::OK || event == Ion::Events::EXE;
}

bool TextFieldDelegateApp::unparsableText(const char * text, Responder * responder) {
  Expression exp = Expression::parse(text);
  if (exp.isUninitialized()) {
    responder->app()->displayWarning(I18n::Message::SyntaxError);
    return true;
  }
  return false;
}

}
