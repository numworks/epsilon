#include "expression_text_field_delegate.h"
#include "apps_container.h"
#include <math.h>

bool ExpressionTextFieldDelegate::textFieldDidReceiveEvent(TextField * textField, Ion::Events::Event event) {
  if (event == Ion::Events::OK && Expression::parse(textField->text()) == nullptr) {
    if (textField->textLength() == 0) {
      return true;
    }
    textField->app()->displayWarning("Attention a la syntaxe jeune padawan");
    return true;
  }
  if (event == Ion::Events::OK &&
    isnan(Expression::parse(textField->text())->approximate(*evaluateContext()))) {
      textField->app()->displayWarning("Relis ton cours de maths, veux tu?");
      return true;
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
  return false;
}
