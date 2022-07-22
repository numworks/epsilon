#ifndef SHARED_TEXT_FIELD_DELEGATE_H
#define SHARED_TEXT_FIELD_DELEGATE_H

#include "text_field_delegate_app.h"
#include <escher/container.h>
#include <escher/text_field_delegate.h>

namespace Shared {

class TextFieldDelegate : public Escher::TextFieldDelegate {
public:
  bool textFieldShouldFinishEditing(Escher::AbstractTextField * textField, Ion::Events::Event event) override;
  bool textFieldDidReceiveEvent(Escher::AbstractTextField * textField, Ion::Events::Event event) override;
protected:
  TextFieldDelegateApp * textFieldDelegateApp() const {
    return static_cast<TextFieldDelegateApp *>(Escher::Container::activeApp());
  }
};

}

#endif
