#ifndef SHARED_TEXT_FIELD_DELEGATE_APP_H
#define SHARED_TEXT_FIELD_DELEGATE_APP_H

#include <poincare/context.h>
#include <escher.h>
#include "input_event_handler_delegate_app.h"
#include "../i18n.h"

class AppsContainer;

namespace Shared {

class TextFieldDelegateApp : public InputEventHandlerDelegateApp, public TextFieldDelegate {
public:
  virtual ~TextFieldDelegateApp() = default;
  virtual Poincare::Context * localContext();
  virtual char XNT();
  bool textFieldShouldFinishEditing(TextField * textField, Ion::Events::Event event) override;
  virtual bool textFieldDidReceiveEvent(TextField * textField, Ion::Events::Event event) override;
protected:
  TextFieldDelegateApp(Container * container, Snapshot * snapshot, ViewController * rootViewController);
  bool fieldDidReceiveEvent(EditableField * field, Responder * responder, Ion::Events::Event event);
  bool isFinishingEvent(Ion::Events::Event event);
  bool isAcceptableText(const char * text, Responder * responder);
  virtual bool isAcceptableExpression(const Poincare::Expression expression, Responder * responder);
};

}

#endif
