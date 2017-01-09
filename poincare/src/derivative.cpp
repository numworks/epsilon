#include <poincare/derivative.h>
#include <poincare/symbol.h>
#include <poincare/float.h>
#include <poincare/x_context.h>

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
  XContext xContext = XContext(&context);
  Symbol xSymbol = Symbol('x');
  Float e = Float(m_args[1]->approximate(context)+k_epsilon);
  xContext.setExpressionForSymbolName(&e, &xSymbol);
  float expressionPlus = m_args[0]->approximate(xContext);
  e = Float(m_args[1]->approximate(context)-k_epsilon);
  xContext.setExpressionForSymbolName(&e, &xSymbol);
  float expressionMinus = m_args[0]->approximate(xContext);
  float growthRate = (expressionPlus - expressionMinus)/(2*k_epsilon);
  return roundf(growthRate/k_precision)*k_precision;
}
