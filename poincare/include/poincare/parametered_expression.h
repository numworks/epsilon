#ifndef POINCARE_PARAMETERED_EXPRESSION_H
#define POINCARE_PARAMETERED_EXPRESSION_H

#include <poincare/expression.h>
#include <ion/unicode/utf8_decoder.h>

namespace Poincare {

/* Parametered expressions are Integral, Derivative, Sum, Product and
 * List Sequence. Their child at index 0 is parametered, and the child
 * at index 1 is the parameter symbol.
 * */

class ParameteredExpressionNode : public ExpressionNode {
public:
  // Expression
  Expression replaceSymbolWithExpression(const SymbolAbstract & symbol, const Expression & expression) override;
  Expression deepReplaceReplaceableSymbols(Context * context, TrinaryBoolean * isCircular, int parameteredAncestorsCount, SymbolicComputation symbolicComputation) override;

  // Expression properties
  int getVariables(Context * context, isVariableTest isVariable, char * variables, int maxSizeVariable, int nextVariableIndex) const override;

protected:
  // Evaluation
  template<typename T> Evaluation<T> approximateFirstChildWithArgument(T x, const ApproximationContext& approximationContext) const;
  template<typename T> T firstChildScalarValueForArgument(T x, const ApproximationContext& approximationContext) const {
    return approximateFirstChildWithArgument(x, approximationContext).toScalar();
  }
  template<typename T> Evaluation<T> approximateExpressionWithArgument(ExpressionNode * child, T x, const ApproximationContext& approximationContext) const;

};

class ParameteredExpression : public Expression {
  friend class ParameteredExpressionNode;
public:
  // ParameteredExpression
  constexpr static int ParameteredChildIndex() { return 0; }
  constexpr static int ParameterChildIndex() { return 1; }

  static bool ParameterText(UnicodeDecoder & varDecoder, size_t * parameterStart, size_t * parameterLength);
  static bool ParameterText(const char * text, const char * * parameterText, size_t * parameterLength);

  // Expression
  /* We sometimes replace 'x' by 'UnknownX' to differentiate between a variable
   * and a function parameter. For instance, when defining the function
   * f(x)=cos(x) in Graph, we want x to be an unknown, instead of having f be
   * the constant function equal to cos(user variable that is named x).
   *
   * In parametered expressions, we do not want to replace all the 'x' with
   * unknowns: for instance, we want to change f(x)=diff(cos(x),x,x) into
   * f(X)=diff(cos(x),x,X), X being an unknown. ReplaceUnknownInExpression does
   * that. */
  Expression replaceSymbolWithExpression(const SymbolAbstract & symbol, const Expression & expression);
  Expression deepReplaceReplaceableSymbols(Context * context, TrinaryBoolean * isCircular, int parameteredAncestorsCount, SymbolicComputation symbolicComputation);
  Symbol parameter();
protected:
  ParameteredExpression(const ParameteredExpressionNode * node) : Expression(node) {}
};

}

#endif
