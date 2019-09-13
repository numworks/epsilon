#ifndef POINCARE_PARAMETERED_EXPRESSION_H
#define POINCARE_PARAMETERED_EXPRESSION_H

#include <poincare/expression.h>

namespace Poincare {

/* Parametered expressions are Integral, Derivative, Sum and Product. Their
 * child at index 0 is parametered, and the child at index 1 is the parameter
 * symbol. */

class ParameteredExpressionNode : public ExpressionNode {
public:
  // Expression
  bool isParameteredExpression() const override { return true; }
  Expression replaceSymbolWithExpression(const SymbolAbstract & symbol, const Expression & expression) override;
};

class ParameteredExpression : public Expression {
  friend class ParameteredExpressionNode;
public:
  // ParameteredExpression
  static constexpr int ParameteredChildIndex() { return 0; }
  static constexpr int ParameterChildIndex() { return 1; }

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
protected:
  ParameteredExpression(const ParameteredExpressionNode * node) : Expression(node) {}
};

}

#endif
