#include "editable_expression_view_delegate.h"

using namespace Poincare;

namespace Shared {

bool EditableExpressionViewDelegate::editableExpressionViewShouldFinishEditing(::EditableExpressionView * editableExpressionView, Ion::Events::Event event) {
  return textFieldAndEditableExpressionViewDelegateApp()->editableExpressionViewShouldFinishEditing(editableExpressionView, event);
}

bool EditableExpressionViewDelegate::editableExpressionViewDidReceiveEvent(::EditableExpressionView * editableExpressionView, Ion::Events::Event event) {
  return textFieldAndEditableExpressionViewDelegateApp()->editableExpressionViewDidReceiveEvent(editableExpressionView, event);
}

bool EditableExpressionViewDelegate::editableExpressionViewDidFinishEditing(EditableExpressionView * editableExpressionView, const char * text, Ion::Events::Event event) {
  return textFieldAndEditableExpressionViewDelegateApp()->editableExpressionViewDidFinishEditing(editableExpressionView, text, event);
}

bool EditableExpressionViewDelegate::editableExpressionViewDidAbortEditing(EditableExpressionView * editableExpressionView, const char * text) {
  return textFieldAndEditableExpressionViewDelegateApp()->editableExpressionViewDidAbortEditing(editableExpressionView, text);
}

bool EditableExpressionViewDelegate::editableExpressionViewDidHandleEvent(EditableExpressionView * editableExpressionView, Ion::Events::Event event, bool returnValue, bool expressionHasChanged) {
  return textFieldAndEditableExpressionViewDelegateApp()->editableExpressionViewDidHandleEvent(editableExpressionView, event, returnValue, expressionHasChanged);
}

void EditableExpressionViewDelegate::editableExpressionViewDidChangeSize(EditableExpressionView * editableExpressionView) {
  return textFieldAndEditableExpressionViewDelegateApp()->editableExpressionViewDidChangeSize(editableExpressionView);
}

Toolbox * EditableExpressionViewDelegate::toolboxForEditableExpressionView(::EditableExpressionView * editableExpressionView) {
  return textFieldAndEditableExpressionViewDelegateApp()->toolboxForEditableExpressionView(editableExpressionView);
}

}
