#include "parameter_text_field_delegate.h"
#include <escher/text_field.h>

namespace Shared {

bool ParameterTextFieldDelegate::textFieldDidReceiveEvent(TextField * textField, Ion::Events::Event event) {
  if (event == Ion::Events::Backspace && !textField->isEditing()) {
    textField->reinitDraftTextBuffer();
    textField->setEditing(true);
    return true;
  }
  return TextFieldDelegate::textFieldDidReceiveEvent(textField, event);
}

}
