#include "text_field_delegate.h"

using namespace Poincare;
using namespace Escher;

namespace Shared {

bool TextFieldDelegate::textFieldShouldFinishEditing(AbstractTextField * textField, Ion::Events::Event event) {
  return textFieldDelegateApp()->textFieldShouldFinishEditing(textField, event);
}

bool TextFieldDelegate::textFieldDidReceiveEvent(AbstractTextField * textField, Ion::Events::Event event) {
  return textFieldDelegateApp()->textFieldDidReceiveEvent(textField, event);
}

}
