#ifndef SHARED_TEXT_FIELD_DELEGATE_APP_H
#define SHARED_TEXT_FIELD_DELEGATE_APP_H

#include <poincare.h>
#include "resettable_app.h"

class AppsContainer;

namespace Shared {

class TextFieldDelegateApp : public ResettableApp, public TextFieldDelegate {
public:
  TextFieldDelegateApp(Container * container, ViewController * rootViewController, I18n::Message name = (I18n::Message)0, I18n::Message upperName = (I18n::Message)0, const Image * icon = nullptr);
  virtual Poincare::Context * localContext();
  AppsContainer * container();
  virtual const char * XNT();
  bool textFieldDidReceiveEvent(TextField * textField, Ion::Events::Event event) override;
  Toolbox * toolboxForTextField(TextField * textField) override;
private:
  bool cursorInToken(TextField * textField, const char * token);
};

}

#endif
