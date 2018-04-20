#include "editable_expression_view_delegate_app.h"
#include "../i18n.h"
#include "../apps_container.h"

using namespace Poincare;

namespace Shared {

EditableExpressionViewDelegateApp::EditableExpressionViewDelegateApp(Container * container, Snapshot * snapshot, ViewController * rootViewController) :
  TextFieldDelegateApp(container, snapshot, rootViewController),
  ExpressionLayoutFieldDelegate()
{
}

bool EditableExpressionViewDelegateApp::expressionLayoutFieldShouldFinishEditing(ExpressionLayoutField * expressionLayoutField, Ion::Events::Event event) {
  return event == Ion::Events::OK || event == Ion::Events::EXE;
}

bool EditableExpressionViewDelegateApp::expressionLayoutFieldDidReceiveEvent(ExpressionLayoutField * expressionLayoutField, Ion::Events::Event event) {
  if (expressionLayoutField->isEditing() && expressionLayoutField->expressionLayoutFieldShouldFinishEditing(event)) {
    if (!expressionLayoutField->expressionViewWithCursor()->expressionView()->expressionLayout()->hasText()) {
      expressionLayoutField->app()->displayWarning(I18n::Message::SyntaxError);
      return true;
    }
    int bufferSize = 256;
    char buffer[bufferSize];
    expressionLayoutField->expressionViewWithCursor()->expressionView()->expressionLayout()->writeTextInBuffer(buffer, bufferSize);
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
    variableBoxController->setExpressionLayoutFieldSender(expressionLayoutField);
    expressionLayoutField->app()->displayModalViewController(variableBoxController, 0.f, 0.f, Metric::PopUpTopMargin, Metric::PopUpLeftMargin, 0, Metric::PopUpRightMargin);
    return true;
  }
  return false;
}

Toolbox * EditableExpressionViewDelegateApp::toolboxForExpressionLayoutField(ExpressionLayoutField * expressionLayoutField) {
  Toolbox * toolbox = container()->mathToolbox();
  static_cast<MathToolbox *>(toolbox)->setSenderAndAction(expressionLayoutField, MathToolbox::actionForExpressionLayoutField);
  return toolbox;
}

}
