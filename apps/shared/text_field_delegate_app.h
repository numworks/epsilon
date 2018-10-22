#ifndef SHARED_TEXT_FIELD_DELEGATE_APP_H
#define SHARED_TEXT_FIELD_DELEGATE_APP_H

#include <poincare/context.h>
#include <escher.h>
#include "../i18n.h"

class AppsContainer;

namespace Shared {

class TextFieldDelegateApp : public ::App, public TextFieldDelegate {
public:
  virtual ~TextFieldDelegateApp() = default;
  virtual Poincare::Context * localContext();
  AppsContainer * container();
  virtual char XNT();
  bool textFieldShouldFinishEditing(TextField * textField, Ion::Events::Event event) override;
  virtual bool textFieldDidReceiveEvent(TextField * textField, Ion::Events::Event event) override;
  Toolbox * toolboxForInputEventHandler(InputEventHandler * textInput) override;
protected:
  TextFieldDelegateApp(Container * container, Snapshot * snapshot, ViewController * rootViewController);
protected:
  bool fieldDidReceiveEvent(EditableField * field, Responder * responder, Ion::Events::Event event);
  void forceEdition(EditableField * field);
  bool isFinishingEvent(Ion::Events::Event event);
  bool unparsableText(const char * text, Responder * responder);
};

}

#endif
