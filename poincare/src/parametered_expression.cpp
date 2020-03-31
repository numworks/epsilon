#include <poincare/parametered_expression.h>
#include <poincare/symbol.h>
#include <string.h>
#include <assert.h>

namespace Poincare {

Expression ParameteredExpressionNode::replaceSymbolWithExpression(const SymbolAbstract & symbol, const Expression & expression) {
  return ParameteredExpression(this).replaceSymbolWithExpression(symbol, expression);
}

Expression ParameteredExpressionNode::deepReplaceReplaceableSymbols(Context * context, bool * didReplace, bool replaceFunctionsOnly, int parameteredAncestorsCount) {
  return ParameteredExpression(this).deepReplaceReplaceableSymbols(context, didReplace, replaceFunctionsOnly, parameteredAncestorsCount);
}

int ParameteredExpressionNode::getVariables(Context * context, isVariableTest isVariable, char * variables, int maxSizeVariable, int nextVariableIndex) const {
  int numberOfVariables = childAtIndex(ParameteredExpression::ParameteredChildIndex())->getVariables(context, isVariable, variables, maxSizeVariable, nextVariableIndex);
  // Handle exception
  if (numberOfVariables < 0) {
    return numberOfVariables;
  }
  /* Remove the parameter symbol from the list of variable if it was added at
   * the previous line */
  const char * parameterName = ParameteredExpression(this).parameter().name();
  for (int i = nextVariableIndex; i < numberOfVariables; i++) {
    if (strcmp(parameterName, &variables[i]) == 0) {
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
  if (symbol.type() != ExpressionNode::Type::Symbol || !parameter().hasSameNameAs(symbol)) {
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

Expression ParameteredExpression::deepReplaceReplaceableSymbols(Context * context, bool * didReplace, bool replaceFunctionsOnly, int parameteredAncestorsCount) {
  /* All children replaceable symbols should be replaced apart from symbols that
   * are parameters in parametered expressions.*/
  int childrenCount = numberOfChildren();
  for (int i = 0; i < childrenCount; i++) {
    if (i == ParameterChildIndex()) {
      // Do not replace symbols in the parameter child
      continue;
    }
    /* In the parametered child, increase the parametered ancestors count so
     * that when replacing symbols, the expressions check that the symbols are
     * not the parametered symbols. */
    bool shouldIncreaseParameteredAncestorsCount = i == ParameteredChildIndex();
    Expression c = childAtIndex(i).deepReplaceReplaceableSymbols(context, didReplace, replaceFunctionsOnly, parameteredAncestorsCount + (shouldIncreaseParameteredAncestorsCount ? 1 : 0));
    if (c.isUninitialized()) { // the expression is circularly defined, escape
      return Expression();
    }
  }
  return *this;
}

Symbol ParameteredExpression::parameter() {
  Expression e = childAtIndex(ParameteredExpression::ParameterChildIndex());
  assert(e.type() == ExpressionNode::Type::Symbol);
  return static_cast<Symbol&>(e);
}

}
