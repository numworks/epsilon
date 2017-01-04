#include <poincare/derivative.h>
#include <poincare/symbol.h>
#include <poincare/float.h>
#include <poincare/context.h>
extern "C" {
#include <assert.h>
#include <math.h>
}

Derivative::Derivative() :
  Function("diff")
{
}

Expression::Type Derivative::type() const {
  return Type::Derivative;
}

Expression * Derivative::cloneWithDifferentOperands(Expression** newOperands,
        int numberOfOperands, bool cloneOperands) const {
  assert(numberOfOperands == 2);
  assert(newOperands != nullptr);
  Derivative * d = new Derivative();
  d->setArgument(newOperands, numberOfOperands, cloneOperands);
  return d;
}

float Derivative::approximate(Context& context) const {
  Symbol xSymbol = Symbol('x');
  const Expression * previousValueForXSymbol = context.expressionForSymbol(&xSymbol);
  Float e = Float(m_args[1]->approximate(context)+k_epsilon);
  context.setExpressionForSymbolName(&e, &xSymbol);
  float expressionPlus = m_args[0]->approximate(context);
  e = Float(m_args[1]->approximate(context)-k_epsilon);
  context.setExpressionForSymbolName(&e, &xSymbol);
  float expressionMinus = m_args[0]->approximate(context);
  float growthRate = (expressionPlus - expressionMinus)/(2*k_epsilon);
  growthRate = roundf(growthRate/k_precision)*k_precision;
  e = Float(NAN);
  context.setExpressionForSymbolName((Expression *)previousValueForXSymbol, &xSymbol);
  return growthRate;
}
