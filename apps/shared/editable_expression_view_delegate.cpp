#include "editable_expression_view_delegate.h"

using namespace Poincare;

namespace Shared {

bool EditableExpressionViewDelegate::editableExpressionViewShouldFinishEditing(::EditableExpressionView * editableExpressionView, Ion::Events::Event event) {
  return textFieldAndEditableExpressionViewDelegateApp()->editableExpressionViewShouldFinishEditing(editableExpressionView, event);
}

bool EditableExpressionViewDelegate::editableExpressionViewDidReceiveEvent(::EditableExpressionView * editableExpressionView, Ion::Events::Event event) {
  return textFieldAndEditableExpressionViewDelegateApp()->editableExpressionViewDidReceiveEvent(editableExpressionView, event);
}

Toolbox * EditableExpressionViewDelegate::toolboxForEditableExpressionView(::EditableExpressionView * editableExpressionView) {
  return textFieldAndEditableExpressionViewDelegateApp()->toolboxForEditableExpressionView(editableExpressionView);
}

}
