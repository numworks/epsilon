#include "parameter_text_field_delegate.h"

using namespace Poincare;

namespace Shared {

bool ParameterTextFieldDelegate::textFieldDidReceiveEvent(::TextField * textField, Ion::Events::Event event) {
  if (event == Ion::Events::Backspace && !textField->isEditing()) {
    textField->setEditing(true);
    return true;
  }
  return TextFieldDelegate::textFieldDidReceiveEvent(textField, event);
}

}
