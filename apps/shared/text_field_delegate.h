#ifndef SHARED_TEXT_FIELD_DELEGATE_H
#define SHARED_TEXT_FIELD_DELEGATE_H

#include <escher.h>
#include "text_field_delegate_app.h"
#include <poincare.h>

namespace Shared {

class TextFieldDelegate : public ::TextFieldDelegate {
public:
  bool textFieldShouldFinishEditing(TextField * textField, Ion::Events::Event event) override;
  bool textFieldDidReceiveEvent(TextField * textField, Ion::Events::Event event) override;
  Toolbox * toolboxForTextInput(TextInput * textInput) override;
private:
  virtual TextFieldDelegateApp * textFieldDelegateApp() = 0;
};

}

#endif
