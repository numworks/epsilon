#include "expression_text_field_delegate.h"
#include <math.h>

bool ExpressionTextFieldDelegate::textFieldDidReceiveEvent(TextField * textField, Ion::Events::Event event) {
  if (event == Ion::Events::Event::ENTER && Expression::parse(textField->text()) == nullptr) {
    if (textField->textLength() == 0) {
      return true;
    }
    textField->app()->displayWarning("Attention a la syntaxe jeune padawan");
    return true;
  }
  if (event == Ion::Events::Event::ENTER &&
    isnan(Expression::parse(textField->text())->approximate(*evaluateContext()))) {
      textField->app()->displayWarning("Relis ton cours de maths, veux tu?");
      return true;
  }
  return false;
}
