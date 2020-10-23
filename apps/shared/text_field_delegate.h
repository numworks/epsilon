#ifndef SHARED_TEXT_FIELD_DELEGATE_H
#define SHARED_TEXT_FIELD_DELEGATE_H

#include "text_field_delegate_app.h"

namespace Shared {

class TextFieldDelegate : public Escher::TextFieldDelegate {
public:
  bool textFieldShouldFinishEditing(Escher::TextField * textField, Ion::Events::Event event) override;
  bool textFieldDidReceiveEvent(Escher::TextField * textField, Ion::Events::Event event) override;
protected:
  TextFieldDelegateApp * textFieldDelegateApp() const {
    return static_cast<TextFieldDelegateApp *>(Escher::Container::activeApp());
  }
};

}

#endif
