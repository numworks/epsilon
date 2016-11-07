#include "expression_text_field_delegate.h"
#include <math.h>

bool ExpressionTextFieldDelegate::textFieldDidReceiveEvent(TextField * textField, Ion::Events::Event event) {
  if (event == Ion::Events::Event::ENTER && Expression::parse(textField->textBuffer()) == nullptr) {
    if (textField->bufferLength() == 0) {
      return true;
    }
    textField->app()->displayWarning("Attention a la syntaxe jeune padawan");
    return true;
  }
  if (event == Ion::Events::Event::ENTER &&
    isnan(Expression::parse(textField->textBuffer())->approximate(*evaluateContext()))) {
      textField->app()->displayWarning("Relis ton cours de maths, veux tu?");
      return true;
  }
  return false;
}
