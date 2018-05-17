#include "expression_layout_field_delegate.h"

using namespace Poincare;

namespace Shared {

bool ExpressionLayoutFieldDelegate::expressionLayoutFieldShouldFinishEditing(ExpressionLayoutField * expressionLayoutField, Ion::Events::Event event) {
  return expressionFieldDelegateApp()->expressionLayoutFieldShouldFinishEditing(expressionLayoutField, event);
}

bool ExpressionLayoutFieldDelegate::expressionLayoutFieldDidReceiveEvent(ExpressionLayoutField * expressionLayoutField, Ion::Events::Event event) {
  return expressionFieldDelegateApp()->expressionLayoutFieldDidReceiveEvent(expressionLayoutField, event);
}

bool ExpressionLayoutFieldDelegate::expressionLayoutFieldDidFinishEditing(ExpressionLayoutField * expressionLayoutField, ExpressionLayout * layout, Ion::Events::Event event) {
  return expressionFieldDelegateApp()->expressionLayoutFieldDidFinishEditing(expressionLayoutField, layout, event);
}

bool ExpressionLayoutFieldDelegate::expressionLayoutFieldDidAbortEditing(ExpressionLayoutField * expressionLayoutField) {
  return expressionFieldDelegateApp()->expressionLayoutFieldDidAbortEditing(expressionLayoutField);
}

void ExpressionLayoutFieldDelegate::expressionLayoutFieldDidChangeSize(ExpressionLayoutField * expressionLayoutField) {
  return expressionFieldDelegateApp()->expressionLayoutFieldDidChangeSize(expressionLayoutField);
}

Toolbox * ExpressionLayoutFieldDelegate::toolboxForExpressionLayoutField(ExpressionLayoutField * expressionLayoutField) {
  return expressionFieldDelegateApp()->toolboxForExpressionLayoutField(expressionLayoutField);
}

}
