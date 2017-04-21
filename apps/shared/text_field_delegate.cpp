#include "text_field_delegate.h"

using namespace Poincare;

namespace Shared {

bool TextFieldDelegate::textFieldShouldFinishEditing(TextField * textField, Ion::Events::Event event) {
  return textFieldDelegateApp()->textFieldShouldFinishEditing(textField, event);
}

bool TextFieldDelegate::textFieldDidReceiveEvent(::TextField * textField, Ion::Events::Event event) {
  return textFieldDelegateApp()->textFieldDidReceiveEvent(textField, event);
}

Toolbox * TextFieldDelegate::toolboxForTextField(::TextField * textField) {
  return textFieldDelegateApp()->toolboxForTextField(textField);
}

}
