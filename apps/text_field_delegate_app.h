#ifndef APPS_TEXT_FIELD_DELEGATE_APP_H
#define APPS_TEXT_FIELD_DELEGATE_APP_H

#include <escher.h>
#include "expression_text_field_delegate.h"

class TextFieldDelegateApp : public ::App, public ExpressionTextFieldDelegate {
public:
  TextFieldDelegateApp(Container * container, ViewController * rootViewController, const char * name = nullptr, const char * upperName = nullptr, const Image * icon = nullptr);
  virtual Context * localContext() override;
};

#endif
