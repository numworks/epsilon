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
  float x = m_args[1]->approximate(context);
  volatile float temp = x + k_epsilon;
  float h = temp - x;
  Float e = Float(x+h);
  xContext.setExpressionForSymbolName(&e, &xSymbol);
  float expressionPlus = m_args[0]->approximate(xContext);
  e = Float(x-h);
  xContext.setExpressionForSymbolName(&e, &xSymbol);
  float expressionMinus = m_args[0]->approximate(xContext);
  float growthRate = (expressionPlus - expressionMinus)/(2*h);
  e = Float(x);
  xContext.setExpressionForSymbolName(&e, &xSymbol);
  float expression = m_args[0]->approximate(xContext);
  float precision = h*h*fabsf(expression);
  float bestPrecision = powf(2.0f, -23.0f);
  precision = precision < bestPrecision ? bestPrecision : precision;
  precision = powf(10.0f, (int)log10f(fabsf(precision)));
  return roundf(growthRate/precision)*precision;
}
