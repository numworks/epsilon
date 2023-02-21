#include "text_field_delegate.h"

#include "text_field_delegate_app.h"

using namespace Poincare;
using namespace Escher;

namespace Shared {

bool TextFieldDelegate::textFieldShouldFinishEditing(
    AbstractTextField *textField, Ion::Events::Event event) {
  return textFieldDelegateApp()->textFieldShouldFinishEditing(textField, event);
}

bool TextFieldDelegate::textFieldDidReceiveEvent(AbstractTextField *textField,
                                                 Ion::Events::Event event) {
  return textFieldDelegateApp()->textFieldDidReceiveEvent(textField, event);
}

TextFieldDelegateApp *TextFieldDelegate::textFieldDelegateApp() const {
  return static_cast<TextFieldDelegateApp *>(Escher::Container::activeApp());
}

}  // namespace Shared
