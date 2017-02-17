#include "expression_text_field_delegate.h"
#include "../apps_container.h"
#include <math.h>
#include <string.h>

using namespace Poincare;

namespace Shared {

const char * ExpressionTextFieldDelegate::XNT() {
  return "x";
}

bool ExpressionTextFieldDelegate::cursorInToken(TextField * textField, const char * token) {
  const char * text = textField->text();
  int location = textField->cursorLocation();
  int tokenLength = strlen(token);
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
  return false;
}

bool ExpressionTextFieldDelegate::textFieldDidReceiveEvent(TextField * textField, Ion::Events::Event event) {
  if (event == Ion::Events::OK && textField->isEditing()) {
    Expression * exp = Expression::parse(textField->text());
    if (exp == nullptr) {
      if (textField->textLength() == 0) {
        return true;
      }
      textField->app()->displayWarning("Attention a la syntaxe jeune padawan");
      return true;
    }
    Expression * evaluation = exp->evaluate(*localContext());
    if (evaluation == nullptr) {
      delete exp;
      textField->app()->displayWarning("Relis ton cours de maths, veux tu?");
      return true;
    } else {
      delete evaluation;
      delete exp;
    }
  }
  if (event == Ion::Events::Toolbox) {
    AppsContainer * appsContainer = (AppsContainer *)textField->app()->container();
    ToolboxController * toolboxController = appsContainer->toolboxController();
    toolboxController->setTextFieldCaller(textField);
    textField->app()->displayModalViewController(toolboxController, 0.f, 0.f, 50, 50, 0, 50);
    return true;
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

}
