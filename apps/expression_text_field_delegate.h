#ifndef APPS_EXPRESSION_TEXT_FIELD_DELEGATE_H
#define APPS_EXPRESSION_TEXT_FIELD_DELEGATE_H

#include <escher.h>

class ExpressionTextFieldDelegate : public TextFieldDelegate {
public:
  bool textFieldDidReceiveEvent(TextField * textField, Ion::Events::Event event) override;
};

#endif
