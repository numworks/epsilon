#include "expression_field_delegate_app.h"
#include "../i18n.h"
#include "../apps_container.h"

using namespace Poincare;

namespace Shared {

ExpressionFieldDelegateApp::ExpressionFieldDelegateApp(Container * container, Snapshot * snapshot, ViewController * rootViewController) :
  TextFieldDelegateApp(container, snapshot, rootViewController),
  ExpressionLayoutFieldDelegate()
{
}

char ExpressionFieldDelegateApp::privateXNT(ExpressionLayoutField * expressionLayoutField) {
  char xntCharFromLayout = expressionLayoutField->XNTChar();
  if (xntCharFromLayout != Ion::Charset::Empty) {
    return xntCharFromLayout;
  }
  return XNT()[0];
}

bool ExpressionFieldDelegateApp::expressionLayoutFieldShouldFinishEditing(ExpressionLayoutField * expressionLayoutField, Ion::Events::Event event) {
  return event == Ion::Events::OK || event == Ion::Events::EXE;
}

bool ExpressionFieldDelegateApp::expressionLayoutFieldDidReceiveEvent(ExpressionLayoutField * expressionLayoutField, Ion::Events::Event event) {
  if (expressionLayoutField->isEditing() && expressionLayoutField->expressionLayoutFieldShouldFinishEditing(event)) {
    if (!expressionLayoutField->hasText()) {
      expressionLayoutField->app()->displayWarning(I18n::Message::SyntaxError);
      return true;
    }
    int bufferSize = 256;
    char buffer[bufferSize];
    expressionLayoutField->writeTextInBuffer(buffer, bufferSize);
    Expression * exp = Expression::parse(buffer);
    if (exp != nullptr) {
      delete exp;
    }
    if (exp == nullptr) {
      expressionLayoutField->app()->displayWarning(I18n::Message::SyntaxError);
      return true;
    }
  }
  if (event == Ion::Events::Var) {
    if (!expressionLayoutField->isEditing()) {
      expressionLayoutField->setEditing(true);
    }
    AppsContainer * appsContainer = (AppsContainer *)expressionLayoutField->app()->container();
    VariableBoxController * variableBoxController = appsContainer->variableBoxController();
    variableBoxController->setSender(expressionLayoutField);
    expressionLayoutField->app()->displayModalViewController(variableBoxController, 0.f, 0.f, Metric::PopUpTopMargin, Metric::PopUpLeftMargin, 0, Metric::PopUpRightMargin);
    return true;
  }
  if (event == Ion::Events::XNT) {
    if (!expressionLayoutField->isEditing()) {
      expressionLayoutField->setEditing(true);
    }
    const char xnt[2] = {privateXNT(expressionLayoutField), 0};
    return expressionLayoutField->handleEventWithText(xnt);
  }
  return false;
}

Toolbox * ExpressionFieldDelegateApp::toolboxForExpressionLayoutField(ExpressionLayoutField * expressionLayoutField) {
  Toolbox * toolbox = container()->mathToolbox();
  toolbox->setSender(expressionLayoutField);
  return toolbox;
}

}
