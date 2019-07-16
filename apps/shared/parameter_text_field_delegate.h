#ifndef SHARED_PARAMETER_TEXT_FIELD_DELEGATE_H
#define SHARED_PARAMETER_TEXT_FIELD_DELEGATE_H

#include "text_field_delegate.h"

namespace Shared {

/* This textField delegate implements the behaviour of textField delegate in
 * parameter menu. */

class ParameterTextFieldDelegate : public TextFieldDelegate {
public:
  bool textFieldDidReceiveEvent(TextField * textField, Ion::Events::Event event) override;
};

}

#endif
