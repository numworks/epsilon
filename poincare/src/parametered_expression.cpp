#include <poincare/parametered_expression.h>
#include <poincare/symbol.h>
#include <string.h>
#include <assert.h>

namespace Poincare {

Expression ParameteredExpressionNode::replaceSymbolWithExpression(const SymbolAbstract & symbol, const Expression & expression) {
  return ParameteredExpression(this).replaceSymbolWithExpression(symbol, expression);
}

int ParameteredExpressionNode::getVariables(Context * context, isVariableTest isVariable, char * variables, int maxSizeVariable, int nextVariableIndex) const {
  int numberOfVariables = childAtIndex(ParameteredExpression::ParameteredChildIndex())->getVariables(context, isVariable, variables, maxSizeVariable, nextVariableIndex);
  // Handle exception
  if (numberOfVariables < 0) {
    return numberOfVariables;
  }
  /* Remove the parameter symbol from the list of variable if it was added at
   * the previous line */
  // Get the parameter symbol
  assert(childAtIndex(ParameteredExpression::ParameterChildIndex())->type() == ExpressionNode::Type::Symbol);
  SymbolNode * parameterChild = static_cast<SymbolNode *>(childAtIndex(ParameteredExpression::ParameterChildIndex()));
  for (int i = nextVariableIndex; i < numberOfVariables; i++) {
    if (strcmp(parameterChild->name(), &variables[i]) == 0) {
      variables[i] = 0;
      numberOfVariables--;
      break;
    }
  }
  nextVariableIndex = numberOfVariables;
  static_assert(ParameteredExpression::ParameteredChildIndex() == 0 && ParameteredExpression::ParameterChildIndex() == 1,
      "ParameteredExpression::getVariables might not be valid");
  for (int i = ParameteredExpression::ParameterChildIndex() + 1; i < numberOfChildren(); i++) {
    int n = childAtIndex(i)->getVariables(context, isVariable, variables, maxSizeVariable, nextVariableIndex);
    // Handle exception
    if (n < 0) {
      return n;
    }
    nextVariableIndex = n;
  }
  return nextVariableIndex;
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
