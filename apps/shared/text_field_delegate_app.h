#ifndef SHARED_TEXT_FIELD_DELEGATE_APP_H
#define SHARED_TEXT_FIELD_DELEGATE_APP_H

#include <poincare/context.h>
#include "input_event_handler_delegate_app.h"
#include <escher/text_field_delegate.h>
#include <apps/i18n.h>

class EditableField;

namespace Shared {

class TextFieldDelegateApp : public InputEventHandlerDelegateApp, public TextFieldDelegate {
public:
  virtual ~TextFieldDelegateApp() = default;
  virtual Poincare::Context * localContext();
  virtual CodePoint XNT() { return 'x'; }
  bool textFieldShouldFinishEditing(TextField * textField, Ion::Events::Event event) override;
  virtual bool textFieldDidReceiveEvent(TextField * textField, Ion::Events::Event event) override;
  bool isAcceptableText(const char * text);
  bool hasUndefinedValue(const char * text, double & value);
protected:
  TextFieldDelegateApp(Snapshot * snapshot, ViewController * rootViewController);
  bool fieldDidReceiveEvent(EditableField * field, Responder * responder, Ion::Events::Event event);
  bool isFinishingEvent(Ion::Events::Event event);
  virtual bool isAcceptableExpression(const Poincare::Expression expression);
  virtual bool storeExpressionAllowed() const { return false; }
  static bool ExpressionCanBeSerialized(const Poincare::Expression expression, bool replaceAns, Poincare::Expression ansExpression);
};

}

#endif
