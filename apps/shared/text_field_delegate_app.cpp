#include "text_field_delegate_app.h"
#include "../apps_container.h"
#include <cmath>
#include <string.h>

using namespace Poincare;

namespace Shared {

TextFieldDelegateApp::TextFieldDelegateApp(Container * container, Snapshot * snapshot, ViewController * rootViewController) :
  ::App(container, snapshot, rootViewController, I18n::Message::Warning),
  TextFieldDelegate()
{
}

Context * TextFieldDelegateApp::localContext() {
  return container()->globalContext();
}

AppsContainer * TextFieldDelegateApp::container() {
  return (AppsContainer *)app()->container();
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
  if (event == Ion::Events::Var) {
    forceEdition(textField);
    return displayVariableBoxController(textField);
  }
  if (event == Ion::Events::XNT) {
    forceEdition(textField);
    const char xnt[2] = {privateXNT(textField), 0};
    return textField->handleEventWithText(xnt);
  }
  return false;
}

Toolbox * TextFieldDelegateApp::toolboxForTextInput(TextInput * textInput) {
  Toolbox * toolbox = container()->mathToolbox();
  toolbox->setSender(textInput);
  return toolbox;
}

/* Protected */

void TextFieldDelegateApp::forceEdition(TextField * textField) {
  if (!textField->isEditing()) {
    textField->setEditing(true);
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

bool TextFieldDelegateApp::displayVariableBoxController(Responder * sender) {
  AppsContainer * appsContainer = (AppsContainer *)sender->app()->container();
  VariableBoxController * variableBoxController = appsContainer->variableBoxController();
  variableBoxController->setSender(sender);
  sender->app()->displayModalViewController(variableBoxController, 0.f, 0.f, Metric::PopUpTopMargin, Metric::PopUpLeftMargin, 0, Metric::PopUpRightMargin);
  return true;
}

/* Private */

char TextFieldDelegateApp::privateXNT(TextField * textField) {
  return textField->XNTChar(XNT());
}

}
