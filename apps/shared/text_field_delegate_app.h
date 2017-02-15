#ifndef SHARED_TEXT_FIELD_DELEGATE_APP_H
#define SHARED_TEXT_FIELD_DELEGATE_APP_H

#include <escher.h>
#include <poincare.h>

class AppsContainer;

namespace Shared {

class TextFieldDelegateApp : public ::App, public TextFieldDelegate {
public:
  TextFieldDelegateApp(Container * container, ViewController * rootViewController, const char * name = nullptr, const char * upperName = nullptr, const Image * icon = nullptr);
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
