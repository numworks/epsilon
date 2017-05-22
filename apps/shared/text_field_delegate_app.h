#ifndef SHARED_TEXT_FIELD_DELEGATE_APP_H
#define SHARED_TEXT_FIELD_DELEGATE_APP_H

#include <poincare.h>
#include <escher.h>
#include "../i18n.h"

class AppsContainer;

namespace Shared {

class TextFieldDelegateApp : public ::App, public TextFieldDelegate {
public:
  TextFieldDelegateApp(Container * container, Snapshot * snapshot, ViewController * rootViewController);
  virtual ~TextFieldDelegateApp() = default;
  virtual Poincare::Context * localContext();
  AppsContainer * container();
  virtual const char * XNT();
  bool textFieldShouldFinishEditing(TextField * textField, Ion::Events::Event event) override;
  bool textFieldDidReceiveEvent(TextField * textField, Ion::Events::Event event) override;
  Toolbox * toolboxForTextField(TextField * textField) override;
private:
  bool cursorInToken(TextField * textField, const char * token);
};

}

#endif
