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

char ExpressionFieldDelegateApp::privateXNT(LayoutField * layoutField) {
  return layoutField->XNTChar(XNT()[0]);
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
    int length = layoutField->serialize(buffer, bufferSize);
    if (length >= bufferSize-1) {
      /* If the buffer is totally full, it is VERY likely that writeTextInBuffer
       * escaped before printing utterly the expression. */
      displayWarning(I18n::Message::SyntaxError);
      return true;
    }
    if (unparsableText(buffer, layoutField)) {
      return true;
    }
  }
  if (event == Ion::Events::Var) {
    forceEdition(layoutField);
    return displayVariableBoxController(layoutField);
  }
  if (event == Ion::Events::XNT) {
    forceEdition(layoutField);
    const char xnt[2] = {privateXNT(layoutField), 0};
    return layoutField->handleEventWithText(xnt);
  }
  return false;
}

Toolbox * ExpressionFieldDelegateApp::toolboxForLayoutField(LayoutField * layoutField) {
  Toolbox * toolbox = container()->mathToolbox();
  toolbox->setSender(layoutField);
  return toolbox;
}

/* Private */

void ExpressionFieldDelegateApp::forceEdition(LayoutField * layoutField) {
  if (!layoutField->isEditing()) {
    layoutField->setEditing(true);
  }
}

}
