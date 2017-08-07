#include <poincare/derivative.h>
#include <poincare/symbol.h>
#include <poincare/complex.h>
#include <cmath>
extern "C" {
#include <assert.h>
#include <float.h>
}

namespace Poincare {

Derivative::Derivative() :
  Function("diff", 2)
{
}

Expression::Type Derivative::type() const {
  return Type::Derivative;
}

Expression * Derivative::cloneWithDifferentOperands(Expression** newOperands,
        int numberOfOperands, bool cloneOperands) const {
  assert(newOperands != nullptr);
  Derivative * d = new Derivative();
  d->setArgument(newOperands, numberOfOperands, cloneOperands);
  return d;
}

Evaluation * Derivative::privateEvaluate(Context& context, AngleUnit angleUnit) const {
  VariableContext xContext = VariableContext('x', &context);
  Symbol xSymbol = Symbol('x');
  Evaluation * xInput = m_args[1]->evaluate(context, angleUnit);
  float x = xInput->toFloat();
  delete xInput;
  Complex e = Complex::Float(x);
  xContext.setExpressionForSymbolName(&e, &xSymbol);
  Evaluation * fInput = m_args[1]->evaluate(xContext, angleUnit);
  float functionValue = fInput->toFloat();
  delete fInput;

  // No complex/matrix version of Derivative
  if (isnan(x) || isnan(functionValue)) {
    return new Complex(Complex::Float(NAN));
  }

  /* Ridders' Algorithm
   * Blibliography:
   * - Press, W. H., Teukolsky, S. A., Vetterling, W. T., & Flannery, B. P.
   * (1992). Numerical recipies in C.
   * - Ridders, C.J.F. 1982, Advances in Engineering Software, vol. 4, no. 2,
   * pp. 75–76. */

  // Initiate hh
  float h = std::fabs(x) < FLT_MIN ? k_minInitialRate : x/1000.0f;
  float f2 = approximateDerivate2(x, h, xContext, angleUnit);
  f2 = std::fabs(f2) < FLT_MIN ? k_minInitialRate : f2;
  float hh = std::sqrt(std::fabs(functionValue/(f2/(std::pow(h,2.0f)))))/10.0f;
  hh = std::fabs(hh) <FLT_MIN ? k_minInitialRate : hh;
  /* Make hh an exactly representable number */
  volatile float temp =  x+hh;
  hh = temp - x;
  /* a is matrix storing the function extrapolations for different stepsizes at
  * different order */
  float a[10][10];
  for (int i = 0; i < 10; i++) {
    for (int j = 0; j < 10; j++) {
      a[i][j] = 1.0f;
    }
  }
  a[0][0] = growthRateAroundAbscissa(x, hh, xContext, angleUnit);
  float err = FLT_MAX;
  float ans = 0.0f;
  float errt = 0.0f;
  /* Loop on i: change the step size */
  for (int i = 1; i < 10; i++) {
    hh /= k_rateStepSize;
    /* Make hh an exactly representable number */
    volatile float temp =  x+hh;
    hh = temp - x;
    a[0][i] = growthRateAroundAbscissa(x, hh, xContext, angleUnit);
    float fac = k_rateStepSize*k_rateStepSize;
    /* Loop on j: compute extrapolation for several orders */
    for (int j = 1; j < 10; j++) {
      a[j][i] = (a[j-1][i]*fac-a[j-1][i-1])/(fac-1.0f);
      fac = k_rateStepSize*k_rateStepSize*fac;
      errt = std::fabs(a[j][i]-a[j-1][i]) > std::fabs(a[j][i]-a[j-1][i-1]) ? std::fabs(a[j][i]-a[j-1][i]) : std::fabs(a[j][i]-a[j-1][i-1]);
      /* Update error and answer if error decreases */
      if (errt < err) {
        err = errt;
        ans = a[j][i];
      }
    }
    /* If higher extrapolation order significantly increases the error, return
     * early */
    if (std::fabs(a[i][i]-a[i-1][i-1]) > 2.0f*err) {
      break;
    }
  }
  /* if the error is too big regarding the value, do not return the answer */
  if (err/ans > k_maxErrorRateOnApproximation || isnan(err)) {
    return new Complex(Complex::Float(NAN));
  }
  if (err < FLT_MIN) {
    return new Complex(Complex::Float(ans));
  }
  err = std::pow(10.0f, (int)std::log10(std::fabs(err))+2.0f);
  return new Complex(Complex::Float(std::round(ans/err)*err));
}

float Derivative::growthRateAroundAbscissa(float x, float h, VariableContext xContext, AngleUnit angleUnit) const {
  Symbol xSymbol = Symbol('x');
  Complex e = Complex::Float(x + h);
  xContext.setExpressionForSymbolName(&e, &xSymbol);
  Evaluation * fInput = m_args[0]->evaluate(xContext, angleUnit);
  float expressionPlus = fInput->toFloat();
  delete fInput;
  e = Complex::Float(x-h);
  xContext.setExpressionForSymbolName(&e, &xSymbol);
  fInput = m_args[0]->evaluate(xContext, angleUnit);
  float expressionMinus = fInput->toFloat();
  delete fInput;
  return (expressionPlus - expressionMinus)/(2*h);
}

float Derivative::approximateDerivate2(float x, float h, VariableContext xContext, AngleUnit angleUnit) const {
  Symbol xSymbol = Symbol('x');
  Complex e = Complex::Float(x + h);
  xContext.setExpressionForSymbolName(&e, &xSymbol);
  Evaluation * fInput = m_args[0]->evaluate(xContext, angleUnit);
  float expressionPlus = fInput->toFloat();
  delete fInput;
  e = Complex::Float(x);
  xContext.setExpressionForSymbolName(&e, &xSymbol);
  fInput = m_args[0]->evaluate(xContext, angleUnit);
  float expression = fInput->toFloat();
  delete fInput;
  e = Complex::Float(x-h);
  xContext.setExpressionForSymbolName(&e, &xSymbol);
  fInput = m_args[0]->evaluate(xContext, angleUnit);
  float expressionMinus = fInput->toFloat();
  delete fInput;
  return expressionPlus - 2.0f*expression + expressionMinus;
}

}

