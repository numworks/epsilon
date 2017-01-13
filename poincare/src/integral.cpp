#include <poincare/integral.h>
#include <poincare/symbol.h>
#include <poincare/float.h>
#include <poincare/context.h>
extern "C" {
#include <assert.h>
#include <math.h>
}

Integral::Integral() :
  Function("int")
{
}

Expression::Type Integral::type() const {
  return Type::Integral;
}

Expression * Integral::cloneWithDifferentOperands(Expression** newOperands,
        int numberOfOperands, bool cloneOperands) const {
  assert(numberOfOperands == 3);
  assert(newOperands != nullptr);
  Integral * i = new Integral();
  i->setArgument(newOperands, numberOfOperands, cloneOperands);
  return i;
}

float Integral::approximate(Context& context) const {
  /* We here use Gauss-Legendre quadrature with n = 5 */
  XContext xContext = XContext(&context);
  static float x[5]={0.1488743389f, 0.4333953941f, 0.6794095682f, 0.8650633666f, 0.9739065285f};
  static float w[5]={0.2955242247f, 0.2692667193f, 0.2190863625f, 0.1494513491f, 0.0666713443f};
  float a = m_args[1]->approximate(context);
  float b = m_args[2]->approximate(context);
  float xm = 0.5f*(a+b);
  float xr = 0.5f*(b-a);
  float result = 0.0f;
  for (int j = 0; j < 5; j++) {
    float dx = xr * x[j];
    result += w[j]*(functionValueAtAbscissa(xm+dx, xContext) + functionValueAtAbscissa(xm-dx, xContext));
  }
  result *= xr;
  return result;
}

float Integral::functionValueAtAbscissa(float x, XContext xContext) const {
  Float e = Float(x);
  Symbol xSymbol = Symbol('x');
  xContext.setExpressionForSymbolName(&e, &xSymbol);
  return m_args[0]->approximate(xContext);
}
