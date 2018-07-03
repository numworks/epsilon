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
  char xntCharFromLayout = layoutField->XNTChar();
  if (xntCharFromLayout != Ion::Charset::Empty) {
    return xntCharFromLayout;
  }
  return XNT()[0];
}

bool ExpressionFieldDelegateApp::layoutFieldShouldFinishEditing(LayoutField * layoutField, Ion::Events::Event event) {
  return event == Ion::Events::OK || event == Ion::Events::EXE;
}

bool ExpressionFieldDelegateApp::layoutFieldDidReceiveEvent(LayoutField * layoutField, Ion::Events::Event event) {
  if (layoutField->isEditing() && layoutField->layoutFieldShouldFinishEditing(event)) {
    if (!layoutField->hasText()) {
      layoutField->app()->displayWarning(I18n::Message::SyntaxError);
      return true;
    }
    char buffer[TextField::maxBufferSize()];
    int bufferSize = TextField::maxBufferSize();
    int length = layoutField->writeTextInBuffer(buffer, bufferSize);
    Expression * exp = Expression::parse(buffer);
    if (exp != nullptr) {
      delete exp;
    }
    if (length >= bufferSize-1) {
      /* If the buffer is totally full, it is VERY likely that writeTextInBuffer
       * escaped before printing utterly the expression. */
      displayWarning(I18n::Message::SyntaxError);
      return true;
    }
    if (exp == nullptr) {
      layoutField->app()->displayWarning(I18n::Message::SyntaxError);
      return true;
    }
  }
  if (event == Ion::Events::Var) {
    if (!layoutField->isEditing()) {
      layoutField->setEditing(true);
    }
    AppsContainer * appsContainer = (AppsContainer *)layoutField->app()->container();
    VariableBoxController * variableBoxController = appsContainer->variableBoxController();
    variableBoxController->setSender(layoutField);
    layoutField->app()->displayModalViewController(variableBoxController, 0.f, 0.f, Metric::PopUpTopMargin, Metric::PopUpLeftMargin, 0, Metric::PopUpRightMargin);
    return true;
  }
  if (event == Ion::Events::XNT) {
    if (!layoutField->isEditing()) {
      layoutField->setEditing(true);
    }
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

}
