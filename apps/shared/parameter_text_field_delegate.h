#ifndef SHARED_PARAMETER_TEXT_FIELD_DELEGATE_H
#define SHARED_PARAMETER_TEXT_FIELD_DELEGATE_H

#include "math_field_delegate.h"

namespace Shared {

/* This textField delegate implements the behaviour of textField delegate in
 * parameter menu. */

class ParameterTextFieldDelegate : public MathTextFieldDelegate {
 public:
  bool textFieldDidReceiveEvent(Escher::AbstractTextField* textField,
                                Ion::Events::Event event) override;
};

}  // namespace Shared

#endif
