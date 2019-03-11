#ifndef POINCARE_PARAMETERED_EXPRESSION_HELPER_H
#define POINCARE_PARAMETERED_EXPRESSION_HELPER_H

#include <poincare/expression.h>

namespace Poincare {

class SymbolAbstract;
class Symbol;

// Parametered expressions are Integral, Derivative, Sum and Product.

namespace ParameteredExpressionHelper {
  /* We sometimes replace 'x' by 'UnknownX' to differentiate between a variable
   * and a function parameter. For instance, when defining the function
   * f(x)=cos(x) in Graph, we want x to be an unknown, instead of having f be
   * the constant function equal to cos(user variable that is named x).
   *
   * In parametered expression, we do not want to replace all the 'x' with
   * unknowns: for instance, we want to change f(x)=diff(cos(x),x,x) into
   * f(X)=diff(cos(x),x,X), X being an unknown. ReplaceUnknownInExpression does
   * that. */
  Expression ReplaceUnknownInExpression(Expression e, const Symbol & symbolToReplace, const Symbol & unknownSymbols);
};

}

#endif
