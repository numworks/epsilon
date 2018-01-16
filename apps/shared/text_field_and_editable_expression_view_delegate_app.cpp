#include "text_field_and_editable_expression_view_delegate_app.h"
#include "../i18n.h"
#include "../apps_container.h"

using namespace Poincare;

namespace Shared {

TextFieldAndEditableExpressionViewDelegateApp::TextFieldAndEditableExpressionViewDelegateApp(Container * container, Snapshot * snapshot, ViewController * rootViewController) :
  TextFieldDelegateApp(container, snapshot, rootViewController),
  EditableExpressionViewDelegate()
{
}

bool TextFieldAndEditableExpressionViewDelegateApp::editableExpressionViewShouldFinishEditing(EditableExpressionView * editableExpressionView, Ion::Events::Event event) {
  return event == Ion::Events::OK || event == Ion::Events::EXE;
}

bool TextFieldAndEditableExpressionViewDelegateApp::editableExpressionViewDidReceiveEvent(EditableExpressionView * editableExpressionView, Ion::Events::Event event) {
  if (editableExpressionView->isEditing() && editableExpressionView->editableExpressionViewShouldFinishEditing(event)) {
    int bufferSize = 256;
    char buffer[bufferSize];
    editableExpressionView->expressionViewWithCursor()->expressionView()->expressionLayout()->writeTextInBuffer(buffer, bufferSize);
    Expression * exp = Expression::parse(buffer);
    if (exp != nullptr) {
      delete exp;
    }
    if (exp == nullptr) {
      editableExpressionView->app()->displayWarning(I18n::Message::SyntaxError);
      return true;
    }
  }
  if (event == Ion::Events::Var) {
    if (!editableExpressionView->isEditing()) {
      editableExpressionView->setEditing(true);
    }
    AppsContainer * appsContainer = (AppsContainer *)editableExpressionView->app()->container();
    VariableBoxController * variableBoxController = appsContainer->variableBoxController();
    variableBoxController->setEditableExpressionViewSender(editableExpressionView);
    editableExpressionView->app()->displayModalViewController(variableBoxController, 0.f, 0.f, Metric::PopUpTopMargin, Metric::PopUpLeftMargin, 0, Metric::PopUpRightMargin);
    return true;
  }
  return false;
}

Toolbox * TextFieldAndEditableExpressionViewDelegateApp::toolboxForEditableExpressionView(EditableExpressionView * editableExpressionView) {
  Toolbox * toolbox = container()->mathToolbox();
  static_cast<MathToolbox *>(toolbox)->setSenderAndAction(editableExpressionView, MathToolbox::actionForEditableExpressionView);
  return toolbox;
}

}
