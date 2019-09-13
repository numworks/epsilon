#include <poincare/parametered_expression.h>
#include <poincare/symbol.h>
#include <string.h>
#include <assert.h>

namespace Poincare {

Expression ParameteredExpressionNode::replaceSymbolWithExpression(const SymbolAbstract & symbol, const Expression & expression) {
  return ParameteredExpression(this).replaceSymbolWithExpression(symbol, expression);
}

Expression ParameteredExpression::replaceSymbolWithExpression(const SymbolAbstract & symbol, const Expression & expression) {
  Expression c = childAtIndex(ParameterChildIndex());
  assert(c.type() == ExpressionNode::Type::Symbol);
  Symbol& parameterChild = static_cast<Symbol &>(c);
  if (symbol.type() != ExpressionNode::Type::Symbol ||
      strcmp(symbol.name(), parameterChild.name()) != 0) {
    // If the symbol is not the parameter, replace normally
    return defaultReplaceSymbolWithExpression(symbol, expression);
  }

  /* If the symbol is the parameter, replace it in all children except
   * in the parameter and the parametered children */
  int childrenCount = numberOfChildren();
  static_assert(ParameteredChildIndex() == 0 && ParameterChildIndex() == 1,
      "ParameteredExpression::replaceSymbolWithExpression might not be valid");
  for (int i = 2; i < childrenCount; i++) {
    childAtIndex(i).replaceSymbolWithExpression(symbol, expression);
  }
  return *this;
}

}
