#include "text_field_delegate_app.h"
#include "../apps_container.h"
#include <math.h>
#include <string.h>

using namespace Poincare;

namespace Shared {

TextFieldDelegateApp::TextFieldDelegateApp(Container * container, ViewController * rootViewController, Descriptor * descriptor) :
  ::App(container, rootViewController, descriptor, I18n::Message::Warning),
  TextFieldDelegate()
{
}

Context * TextFieldDelegateApp::localContext() {
  return container()->globalContext();
}

AppsContainer * TextFieldDelegateApp::container() {
  return (AppsContainer *)app()->container();
}

const char * TextFieldDelegateApp::XNT() {
  return "x";
}

bool TextFieldDelegateApp::cursorInToken(TextField * textField, const char * token) {
  /* TODO: find a way to return true when cursor is in token(?,,). Warning: be
   * able to differenciate sum(int(x,1,2),3,4) and sum(2*n,2,3) */
  return false;
}

bool TextFieldDelegateApp::textFieldShouldFinishEditing(TextField * textField, Ion::Events::Event event) {
  return event == Ion::Events::OK || event == Ion::Events::EXE;
}

bool TextFieldDelegateApp::textFieldDidReceiveEvent(TextField * textField, Ion::Events::Event event) {
  if (textField->textFieldShouldFinishEditing(event) && textField->isEditing()) {
    Expression * exp = Expression::parse(textField->text());
    bool invalidText = (exp == nullptr || !exp->hasValidNumberOfArguments());
    if (exp != nullptr) {
      delete exp;
    }
    if (invalidText) {
      textField->app()->displayWarning(I18n::Message::SyntaxError);
      return true;
    }
  }
  if (event == Ion::Events::Var) {
    AppsContainer * appsContainer = (AppsContainer *)textField->app()->container();
    VariableBoxController * variableBoxController = appsContainer->variableBoxController();
    variableBoxController->setTextFieldCaller(textField);
    textField->app()->displayModalViewController(variableBoxController, 0.f, 0.f, Metric::PopUpTopMargin, Metric::PopUpLeftMargin, 0, Metric::PopUpRightMargin);
    return true;
  }
  if (event == Ion::Events::XNT) {
    if (!textField->isEditing()) {
      textField->setEditing(true);
    }
    if (cursorInToken(textField, "sum") || cursorInToken(textField, "product")) {
      textField->insertTextAtLocation("n", textField->cursorLocation());
      textField->setCursorLocation(textField->cursorLocation()+strlen("n"));
      return true;
    }
    textField->insertTextAtLocation(XNT(), textField->cursorLocation());
    textField->setCursorLocation(textField->cursorLocation()+strlen(XNT()));
    return true;
  }
  return false;
}

Toolbox * TextFieldDelegateApp::toolboxForTextField(TextField * textField) {
  return container()->mathToolbox();
}

}
