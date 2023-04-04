#include <assert.h>
#include <ion/unicode/utf8_decoder.h>
#include <poincare/parametered_expression.h>
#include <poincare/symbol.h>
#include <poincare/variable_context.h>
#include <string.h>

#include "parsing/tokenizer.h"

namespace Poincare {

Expression ParameteredExpressionNode::replaceSymbolWithExpression(
    const SymbolAbstract& symbol, const Expression& expression) {
  return ParameteredExpression(this).replaceSymbolWithExpression(symbol,
                                                                 expression);
}

Expression ParameteredExpressionNode::deepReplaceReplaceableSymbols(
    Context* context, TrinaryBoolean* isCircular, int parameteredAncestorsCount,
    SymbolicComputation symbolicComputation) {
  return ParameteredExpression(this).deepReplaceReplaceableSymbols(
      context, isCircular, parameteredAncestorsCount, symbolicComputation);
}

int ParameteredExpressionNode::getVariables(Context* context,
                                            isVariableTest isVariable,
                                            char* variables,
                                            int maxSizeVariable,
                                            int nextVariableIndex) const {
  assert(isParameteredExpression());
  int numberOfVariables =
      childAtIndex(ParameteredExpression::ParameteredChildIndex())
          ->getVariables(context, isVariable, variables, maxSizeVariable,
                         nextVariableIndex);
  // Handle exception
  if (numberOfVariables < 0) {
    return numberOfVariables;
  }
  /* Remove the parameter symbol from the list of variable if it was added at
   * the previous line */
  const char* parameterName = ParameteredExpression(this).parameter().name();
  for (int index = nextVariableIndex * maxSizeVariable;
       index < numberOfVariables * maxSizeVariable; index += maxSizeVariable) {
    if (strcmp(parameterName, &variables[index]) == 0) {
      memmove(&variables[index], &variables[index + maxSizeVariable],
              (numberOfVariables - nextVariableIndex) * maxSizeVariable);
      numberOfVariables--;
      break;
    }
  }
  if (numberOfVariables < Expression::k_maxNumberOfVariables) {
    variables[numberOfVariables * maxSizeVariable] = '\0';
  }
  nextVariableIndex = numberOfVariables;
  static_assert(ParameteredExpression::ParameteredChildIndex() == 0 &&
                    ParameteredExpression::ParameterChildIndex() == 1,
                "ParameteredExpression::getVariables might not be valid");
  int childrenNumber = numberOfChildren();
  for (int i = ParameteredExpression::ParameterChildIndex() + 1;
       i < childrenNumber; i++) {
    int n = childAtIndex(i)->getVariables(context, isVariable, variables,
                                          maxSizeVariable, nextVariableIndex);
    // Handle exception
    if (n < 0) {
      return n;
    }
    nextVariableIndex = n;
  }
  return nextVariableIndex;
}

template <typename T>
Evaluation<T> ParameteredExpressionNode::approximateExpressionWithArgument(
    ExpressionNode* expression, T x,
    const ApproximationContext& approximationContext) const {
  assert(childAtIndex(1)->type() == Type::Symbol);
  Symbol symbol = Symbol(static_cast<SymbolNode*>(childAtIndex(1)));
  VariableContext variableContext =
      VariableContext(symbol.name(), approximationContext.context());
  variableContext.setApproximationForVariable<T>(x);
  // Here we cannot use Expression::approximateWithValueForSymbol which would
  // reset the sApproximationEncounteredComplex flag
  ApproximationContext childContext = approximationContext;
  childContext.setContext(&variableContext);
  return expression->approximate(T(), childContext);
}

template <typename T>
Evaluation<T> ParameteredExpressionNode::approximateFirstChildWithArgument(
    T x, const ApproximationContext& approximationContext) const {
  return approximateExpressionWithArgument(childAtIndex(0), x,
                                           approximationContext);
}

bool ParameteredExpression::ParameterText(UnicodeDecoder& varDecoder,
                                          size_t* parameterStart,
                                          size_t* parameterLength) {
  /* Find the beginning of the parameter. Count parentheses to handle the
   * presence of functions with several parameters in the parametered
   * expression. */
  CodePoint c = UCodePointUnknown;
  bool variableFound = false;
  int cursorLevel = 0;
  while (c != UCodePointNull && cursorLevel >= 0 && !variableFound) {
    c = varDecoder.nextCodePoint();
    switch (c) {
      case '(':
      case '{':
      case UCodePointLeftSystemParenthesis:
        cursorLevel++;
        break;
      case ')':
      case '}':
      case UCodePointRightSystemParenthesis:
        cursorLevel--;
        break;
      case ',':
        variableFound = cursorLevel == 0;
        break;
    }
  }
  if (!variableFound || c == UCodePointNull) {
    return false;
  }

  size_t startOfVariable = varDecoder.position();
  do {
    c = varDecoder.nextCodePoint();
  } while (c != UCodePointNull && c != ',' && c != ')');
  size_t endOfVariable = varDecoder.position();
  varDecoder.unsafeSetPosition(startOfVariable);
  do {
    // Skip whitespaces
    c = varDecoder.nextCodePoint();
  } while (c == ' ');
  varDecoder.previousCodePoint();
  startOfVariable = varDecoder.position();
  size_t lengthOfVariable = endOfVariable - startOfVariable - 1;

  if (!Tokenizer::CanBeCustomIdentifier(varDecoder, lengthOfVariable)) {
    return false;
  }
  *parameterLength = lengthOfVariable;
  *parameterStart = startOfVariable;
  return true;
}

bool ParameteredExpression::ParameterText(const char* text,
                                          const char** parameterText,
                                          size_t* parameterLength) {
  UTF8Decoder decoder(text);
  size_t parameterStart = *parameterText - text;
  bool result = ParameterText(decoder, &parameterStart, parameterLength);
  *parameterText = parameterStart + text;
  return result;
}

Expression ParameteredExpression::replaceSymbolWithExpression(
    const SymbolAbstract& symbol, const Expression& expression) {
  if (symbol.type() != ExpressionNode::Type::Symbol ||
      !parameter().hasSameNameAs(symbol)) {
    // If the symbol is not the parameter, replace normally
    return deepReplaceSymbolWithExpression(symbol, expression);
  }

  /* If the symbol is the parameter, replace it in all children except
   * in the parameter and the parametered children */
  int childrenCount = numberOfChildren();
  static_assert(
      ParameteredChildIndex() == 0 && ParameterChildIndex() == 1,
      "ParameteredExpression::replaceSymbolWithExpression might not be valid");
  for (int i = 2; i < childrenCount; i++) {
    childAtIndex(i).replaceSymbolWithExpression(symbol, expression);
  }
  return *this;
}

Expression ParameteredExpression::deepReplaceReplaceableSymbols(
    Context* context, TrinaryBoolean* isCircular, int parameteredAncestorsCount,
    SymbolicComputation symbolicComputation) {
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
    childAtIndex(i).deepReplaceReplaceableSymbols(
        context, isCircular,
        parameteredAncestorsCount +
            (shouldIncreaseParameteredAncestorsCount ? 1 : 0),
        symbolicComputation);
    if (*isCircular == TrinaryBoolean::True) {
      // the expression is circularly defined, escape
      return *this;
    }
  }
  return *this;
}

Symbol ParameteredExpression::parameter() {
  Expression e = childAtIndex(ParameteredExpression::ParameterChildIndex());
  assert(e.type() == ExpressionNode::Type::Symbol);
  return static_cast<Symbol&>(e);
}

template Evaluation<float>
ParameteredExpressionNode::approximateFirstChildWithArgument(
    float x, const ApproximationContext& approximationContext) const;
template Evaluation<double>
ParameteredExpressionNode::approximateFirstChildWithArgument(
    double x, const ApproximationContext& approximationContext) const;
template Evaluation<float>
ParameteredExpressionNode::approximateExpressionWithArgument(
    ExpressionNode* expr, float x,
    const ApproximationContext& approximationContext) const;
template Evaluation<double>
ParameteredExpressionNode::approximateExpressionWithArgument(
    ExpressionNode* expr, double x,
    const ApproximationContext& approximationContext) const;

}  // namespace Poincare
