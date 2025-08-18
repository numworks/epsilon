#ifndef POINCARE_PARAMETERED_EXPRESSION_H
#define POINCARE_PARAMETERED_EXPRESSION_H

#include <omg/utf8_decoder.h>

#include "old_expression.h"

namespace Poincare {

/* Parametered expressions are Integral, Derivative, Sum, Product and
 * OList Sequence. Their child at index 0 is parametered, and the child
 * at index 1 is the parameter symbol.
 * */

class ParameteredExpressionNode : public ExpressionNode {
 public:
  // OExpression
  OExpression replaceSymbolWithExpression(
      const SymbolAbstract& symbol, const OExpression& expression) override;

  // OExpression properties
  int getVariables(Context* context, isVariableTest isVariable, char* variables,
                   int maxSizeVariable, int nextVariableIndex) const override;

 protected:
  // Evaluation
  template <typename T>
  Evaluation<T> approximateFirstChildWithArgument(
      T x, const ApproximationContext& approximationContext) const;
  template <typename T>
  T firstChildScalarValueForArgument(
      T x, const ApproximationContext& approximationContext) const {
    return approximateFirstChildWithArgument(x, approximationContext)
        .toRealScalar();
  }
  template <typename T>
  Evaluation<T> approximateExpressionWithArgument(
      ExpressionNode* child, T x,
      const ApproximationContext& approximationContext) const;
};

class ParameteredExpression : public OExpression {
  friend class ParameteredExpressionNode;

 public:
  // ParameteredExpression
  constexpr static int ParameteredChildIndex() { return 0; }
  constexpr static int ParameterChildIndex() { return 1; }

  static bool ParameterText(UnicodeDecoder& varDecoder, size_t* parameterStart,
                            size_t* parameterLength);
  static bool ParameterText(const char* text, const char** parameterText,
                            size_t* parameterLength);

  // OExpression
  /* We sometimes replace 'x' by 'UnknownX' to differentiate between a variable
   * and a function parameter. For instance, when defining the function
   * f(x)=cos(x) in Graph, we want x to be an unknown, instead of having f be
   * the constant function equal to cos(user variable that is named x).
   *
   * In parametered expressions, we do not want to replace all the 'x' with
   * unknowns: for instance, we want to change f(x)=diff(cos(x),x,x) into
   * f(X)=diff(cos(x),x,X), X being an unknown. ReplaceUnknownInExpression does
   * that. */
  OExpression replaceSymbolWithExpression(const SymbolAbstract& symbol,
                                          const OExpression& expression);
  Symbol parameter();

 protected:
  ParameteredExpression(const ParameteredExpressionNode* node)
      : OExpression(node) {}
};

}  // namespace Poincare

#endif
