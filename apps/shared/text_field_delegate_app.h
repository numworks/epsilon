#ifndef SHARED_TEXT_FIELD_DELEGATE_APP_H
#define SHARED_TEXT_FIELD_DELEGATE_APP_H

#include <poincare/context.h>
#include "input_event_handler_delegate_app.h"
#include <escher/text_field_delegate.h>
#include <apps/i18n.h>
#include "../apps_container.h"

class EditableField;

namespace Shared {

class TextFieldDelegateApp : public InputEventHandlerDelegateApp, public TextFieldDelegate {
public:
  virtual ~TextFieldDelegateApp() = default;
  Poincare::Context * localContext() override;
  virtual bool XNTCanBeOverriden() const { return true; }
  virtual CodePoint XNT() { return 'x'; }
  virtual void textFieldDidReceiveNoneXNTEvent() override { AppsContainer::sharedAppsContainer()->resetXNT(); }
  bool textFieldShouldFinishEditing(TextField * textField, Ion::Events::Event event) override;
  bool textFieldDidReceiveEvent(TextField * textField, Ion::Events::Event event) override;
  bool isAcceptableText(const char * text);
  template<typename T>
  bool hasUndefinedValue(const char * text, T & value, bool enablePlusInfinity = false, bool enableMinusInfinity = false);
protected:
  TextFieldDelegateApp(Snapshot * snapshot, ViewController * rootViewController);
  bool fieldDidReceiveEvent(EditableField * field, Responder * responder, Ion::Events::Event event);
  bool isFinishingEvent(Ion::Events::Event event);
  virtual bool isAcceptableExpression(const Poincare::Expression expression);
  static bool ExpressionCanBeSerialized(const Poincare::Expression expression, bool replaceAns, Poincare::Expression ansExpression, Poincare::Context * context);
};

}

#endif
