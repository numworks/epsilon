#include "expression_text_field_delegate.h"
#include <poincare.h>

bool ExpressionTextFieldDelegate::textFieldDidReceiveEvent(TextField * textField, Ion::Events::Event event) {
  if (event == Ion::Events::Event::ENTER && Expression::parse(textField->textBuffer()) == nullptr) {
    if (textField->bufferLength() == 0) {
      return true;
    }
    textField->app()->displayWarning("Attention a la syntaxe jeune padawan");
    return true;
  }
  return false;
}
