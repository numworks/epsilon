#ifndef SHARED_TEXT_FIELD_DELEGATE_H
#define SHARED_TEXT_FIELD_DELEGATE_H

#include <escher/container.h>
#include <escher/text_field_delegate.h>

namespace Shared {

class TextFieldDelegateApp;

class TextFieldDelegate : public Escher::TextFieldDelegate {
 public:
  bool textFieldShouldFinishEditing(Escher::AbstractTextField* textField,
                                    Ion::Events::Event event) override;
  bool textFieldDidReceiveEvent(Escher::AbstractTextField* textField,
                                Ion::Events::Event event) override;

 protected:
  TextFieldDelegateApp* textFieldDelegateApp() const;
};

}  // namespace Shared

#endif
