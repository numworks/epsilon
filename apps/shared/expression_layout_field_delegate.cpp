#include "expression_layout_field_delegate.h"

using namespace Poincare;

namespace Shared {

bool ExpressionLayoutFieldDelegate::expressionLayoutFieldShouldFinishEditing(ExpressionLayoutField * expressionLayoutField, Ion::Events::Event event) {
  return editableExpressionViewDelegateApp()->expressionLayoutFieldShouldFinishEditing(expressionLayoutField, event);
}

bool ExpressionLayoutFieldDelegate::expressionLayoutFieldDidReceiveEvent(ExpressionLayoutField * expressionLayoutField, Ion::Events::Event event) {
  return editableExpressionViewDelegateApp()->expressionLayoutFieldDidReceiveEvent(expressionLayoutField, event);
}

bool ExpressionLayoutFieldDelegate::expressionLayoutFieldDidFinishEditing(ExpressionLayoutField * expressionLayoutField, const char * text, Ion::Events::Event event) {
  return editableExpressionViewDelegateApp()->expressionLayoutFieldDidFinishEditing(expressionLayoutField, text, event);
}

bool ExpressionLayoutFieldDelegate::expressionLayoutFieldDidAbortEditing(ExpressionLayoutField * expressionLayoutField) {
  return editableExpressionViewDelegateApp()->expressionLayoutFieldDidAbortEditing(expressionLayoutField);
}

void ExpressionLayoutFieldDelegate::expressionLayoutFieldDidChangeSize(ExpressionLayoutField * expressionLayoutField) {
  return editableExpressionViewDelegateApp()->expressionLayoutFieldDidChangeSize(expressionLayoutField);
}

Toolbox * ExpressionLayoutFieldDelegate::toolboxForExpressionLayoutField(ExpressionLayoutField * expressionLayoutField) {
  return editableExpressionViewDelegateApp()->toolboxForExpressionLayoutField(expressionLayoutField);
}

}
