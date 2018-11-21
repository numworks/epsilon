#include "expression_field_delegate_app.h"
#include <escher.h>
#include "../i18n.h"
#include "../apps_container.h"

using namespace Poincare;

namespace Shared {

ExpressionFieldDelegateApp::ExpressionFieldDelegateApp(Container * container, Snapshot * snapshot, ViewController * rootViewController) :
  TextFieldDelegateApp(container, snapshot, rootViewController),
  LayoutFieldDelegate()
{
}

bool ExpressionFieldDelegateApp::layoutFieldShouldFinishEditing(LayoutField * layoutField, Ion::Events::Event event) {
  return isFinishingEvent(event);
}

bool ExpressionFieldDelegateApp::layoutFieldDidReceiveEvent(LayoutField * layoutField, Ion::Events::Event event) {
  if (layoutField->isEditing() && layoutField->shouldFinishEditing(event)) {
    if (!layoutField->hasText()) {
      layoutField->app()->displayWarning(I18n::Message::SyntaxError);
      return true;
    }
    char buffer[TextField::maxBufferSize()];
    int bufferSize = TextField::maxBufferSize();
    int length = layoutField->layout().serializeForParsing(buffer, bufferSize);
    if (length >= bufferSize-1) {
      /* If the buffer is totally full, it is VERY likely that writeTextInBuffer
       * escaped before printing utterly the expression. */
      displayWarning(I18n::Message::SyntaxError);
      return true;
    }
    if (!isAcceptableText(buffer)) {
      displayWarning(I18n::Message::SyntaxError);
      return true;
    }
  }
  if (fieldDidReceiveEvent(layoutField, layoutField, event)) {
    return true;
  }
  return false;
}

}
