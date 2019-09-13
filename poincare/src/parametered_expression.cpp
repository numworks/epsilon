#include <poincare/parametered_expression.h>
#include <poincare/symbol.h>
#include <string.h>
#include <assert.h>

namespace Poincare {

Expression ParameteredExpressionNode::replaceUnknown(const Symbol & symbol, const Symbol & unknownSymbol) {
  return ParameteredExpression(this).replaceUnknown(symbol, unknownSymbol);
}

Expression ParameteredExpression::replaceUnknown(const Symbol & symbolToReplace, const Symbol & unknownSymbol) {
  Expression c = childAtIndex(ParameterChildIndex());
  assert(c.type() == ExpressionNode::Type::Symbol);
  Symbol& parameterChild = static_cast<Symbol &>(c);
  if (strcmp(parameterChild.name(), symbolToReplace.name()) != 0) {
    // If the unknown is not the parameter, replace normally
    return defaultReplaceUnknown(symbolToReplace, unknownSymbol);
  }

  /* If the unknown is the parameter, replace the unknown in all children except
   * in the parameter and the parametered children */
  int childrenCount = numberOfChildren();
  static_assert(ParameteredChildIndex() == 0 && ParameterChildIndex() == 1,
      "ParameteredExpression::replaceUnknown might not be valid");
  for (int i = 2; i < childrenCount; i++) {
    childAtIndex(i).replaceUnknown(symbolToReplace, unknownSymbol);
  }
  return *this;
}

}
