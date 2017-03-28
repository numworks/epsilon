#include "text_field_delegate_app.h"
#include "../apps_container.h"
#include <math.h>
#include <string.h>

using namespace Poincare;

namespace Shared {

TextFieldDelegateApp::TextFieldDelegateApp(Container * container, ViewController * rootViewController, I18n::Message name, I18n::Message upperName, const Image * icon) :
  ::App(container, rootViewController, name, upperName, icon, I18n::Message::Warning),
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
  const char * text = textField->text();
  int location = textField->cursorLocation();
  int tokenLength = strlen(token);
  while (location >= 0) {
    while (text[location] != '(') {
      location --;
    }
    if (location - tokenLength < 0) {
      return false;
    }
    char previousToken[10];
    strlcpy(previousToken, text+location-tokenLength, tokenLength+1);
    if (strcmp(previousToken, token) == 0) {
      return true;
    }
    location--;
  }
  return false;
}

bool TextFieldDelegateApp::textFieldDidReceiveEvent(TextField * textField, Ion::Events::Event event) {
  if ((event == Ion::Events::OK || event == Ion::Events::EXE) && textField->isEditing()) {
    Expression * exp = Expression::parse(textField->text());
    if (exp == nullptr || !exp->hasValidNumberOfArguments()) {
      if (textField->textLength() == 0) {
        return true;
      }
      textField->app()->displayWarning(I18n::Message::SyntaxError);
      return true;
    }
  }
  if (event == Ion::Events::Var) {
    AppsContainer * appsContainer = (AppsContainer *)textField->app()->container();
    VariableBoxController * variableBoxController = appsContainer->variableBoxController();
    variableBoxController->setTextFieldCaller(textField);
    textField->app()->displayModalViewController(variableBoxController, 0.f, 0.f, 50, 50, 0, 50);
    return true;
  }
  if (event == Ion::Events::XNT) {
    if (!textField->isEditing()) {
      textField->setEditing(true);
      textField->setText("");
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
