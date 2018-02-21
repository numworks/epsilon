#ifndef SHARED_TEXT_FIELD_DELEGATE_APP_H
#define SHARED_TEXT_FIELD_DELEGATE_APP_H

#include <poincare.h>
#include <escher.h>
#include "../i18n.h"

class AppsContainer;

namespace Shared {

class TextFieldDelegateApp : public ::App, public TextFieldDelegate {
public:
  virtual ~TextFieldDelegateApp() = default;
  virtual Poincare::Context * localContext();
  AppsContainer * container();
  virtual const char * XNT();
  bool textFieldShouldFinishEditing(TextField * textField, Ion::Events::Event event) override;
  virtual bool textFieldDidReceiveEvent(TextField * textField, Ion::Events::Event event) override;
  Toolbox * toolboxForTextInput(TextInput * textInput) override;
protected:
  TextFieldDelegateApp(Container * container, Snapshot * snapshot, ViewController * rootViewController);
private:
  const char * privateXNT(TextField * textField);
};

}

#endif
