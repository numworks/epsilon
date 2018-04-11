#include "editable_expression_view_delegate_app.h"
#include "../i18n.h"
#include "../apps_container.h"

using namespace Poincare;

namespace Shared {

EditableExpressionViewDelegateApp::EditableExpressionViewDelegateApp(Container * container, Snapshot * snapshot, ViewController * rootViewController) :
  TextFieldDelegateApp(container, snapshot, rootViewController),
  ScrollableExpressionViewWithCursorDelegate()
{
}

bool EditableExpressionViewDelegateApp::scrollableExpressionViewWithCursorShouldFinishEditing(ScrollableExpressionViewWithCursor * scrollableExpressionViewWithCursor, Ion::Events::Event event) {
  return event == Ion::Events::OK || event == Ion::Events::EXE;
}

bool EditableExpressionViewDelegateApp::scrollableExpressionViewWithCursorDidReceiveEvent(ScrollableExpressionViewWithCursor * scrollableExpressionViewWithCursor, Ion::Events::Event event) {
  if (scrollableExpressionViewWithCursor->isEditing() && scrollableExpressionViewWithCursor->scrollableExpressionViewWithCursorShouldFinishEditing(event)) {
    if (!scrollableExpressionViewWithCursor->expressionViewWithCursor()->expressionView()->expressionLayout()->hasText()) {
      scrollableExpressionViewWithCursor->app()->displayWarning(I18n::Message::SyntaxError);
      return true;
    }
    int bufferSize = 256;
    char buffer[bufferSize];
    scrollableExpressionViewWithCursor->expressionViewWithCursor()->expressionView()->expressionLayout()->writeTextInBuffer(buffer, bufferSize);
    Expression * exp = Expression::parse(buffer);
    if (exp != nullptr) {
      delete exp;
    }
    if (exp == nullptr) {
      scrollableExpressionViewWithCursor->app()->displayWarning(I18n::Message::SyntaxError);
      return true;
    }
  }
  if (event == Ion::Events::Var) {
    if (!scrollableExpressionViewWithCursor->isEditing()) {
      scrollableExpressionViewWithCursor->setEditing(true);
    }
    AppsContainer * appsContainer = (AppsContainer *)scrollableExpressionViewWithCursor->app()->container();
    VariableBoxController * variableBoxController = appsContainer->variableBoxController();
    variableBoxController->setScrollableExpressionViewWithCursorSender(scrollableExpressionViewWithCursor);
    scrollableExpressionViewWithCursor->app()->displayModalViewController(variableBoxController, 0.f, 0.f, Metric::PopUpTopMargin, Metric::PopUpLeftMargin, 0, Metric::PopUpRightMargin);
    return true;
  }
  return false;
}

Toolbox * EditableExpressionViewDelegateApp::toolboxForScrollableExpressionViewWithCursor(ScrollableExpressionViewWithCursor * scrollableExpressionViewWithCursor) {
  Toolbox * toolbox = container()->mathToolbox();
  static_cast<MathToolbox *>(toolbox)->setSenderAndAction(scrollableExpressionViewWithCursor, MathToolbox::actionForScrollableExpressionViewWithCursor);
  return toolbox;
}

}
