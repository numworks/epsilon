#include <poincare/derivative.h>
#include <poincare/symbol.h>
#include <poincare/complex.h>
#include <poincare/simplification_engine.h>
#include <poincare/undefined.h>
#include <cmath>
extern "C" {
#include <assert.h>
#include <float.h>
}

namespace Poincare {

Expression::Type Derivative::type() const {
  return Type::Derivative;
}

Expression * Derivative::clone() const {
  Derivative * a = new Derivative(m_operands, true);
  return a;
}

Expression * Derivative::shallowReduce(Context& context, AngleUnit angleUnit) {
  Expression * e = Expression::shallowReduce(context, angleUnit);
  if (e != this) {
    return e;
  }
#if MATRIX_EXACT_REDUCING
  if (operand(0)->type() == Type::Matrix || operand(1)->type() == Type::Matrix) {
    return replaceWith(new Undefined(), true);
  }
#endif
  // TODO: to be implemented diff(+) -> +diff() etc
  return this;
}

template<typename T>
Expression * Derivative::templatedEvaluate(Context& context, AngleUnit angleUnit) const {
  static T min = sizeof(T) == sizeof(double) ? DBL_MIN : FLT_MIN;
  static T max = sizeof(T) == sizeof(double) ? DBL_MAX : FLT_MAX;
  VariableContext<T> xContext = VariableContext<T>('x', &context);
  Symbol xSymbol('x');
  Expression * xInput = operand(1)->evaluate<T>(context, angleUnit);
  T x = xInput->type() == Type::Complex ? static_cast<Complex<T> *>(xInput)->toScalar() : NAN;
  delete xInput;
  Complex<T> e = Complex<T>::Float(x);
  xContext.setExpressionForSymbolName(&e, &xSymbol, xContext);
  Expression * fInput = operand(0)->evaluate<T>(xContext, angleUnit);
  T functionValue = fInput->type() == Type::Complex ? static_cast<Complex<T> *>(fInput)->toScalar() : NAN;
  delete fInput;

  // No complex/matrix version of Derivative
  if (std::isnan(x) || std::isnan(functionValue)) {
    return new Complex<T>(Complex<T>::Float(NAN));
  }

  /* Ridders' Algorithm
   * Blibliography:
   * - Press, W. H., Teukolsky, S. A., Vetterling, W. T., & Flannery, B. P.
   * (1992). Numerical recipies in C.
   * - Ridders, C.J.F. 1982, Advances in Engineering Software, vol. 4, no. 2,
   * pp. 75–76. */

  // Initialize hh
  T h = std::fabs(x) < min ? k_minInitialRate : x/1000;
  T f2 = approximateDerivate2(x, h, xContext, angleUnit);
  f2 = std::fabs(f2) < min ? k_minInitialRate : f2;
  T hh = std::sqrt(std::fabs(functionValue/(f2/(std::pow(h,2)))))/10;
  hh = std::fabs(hh) < min ? k_minInitialRate : hh;
  /* Make hh an exactly representable number */
  volatile T temp =  x+hh;
  hh = temp - x;
  /* a is matrix storing the function extrapolations for different stepsizes at
  * different order */
  T a[10][10];
  for (int i = 0; i < 10; i++) {
    for (int j = 0; j < 10; j++) {
      a[i][j] = 1;
    }
  }
  a[0][0] = growthRateAroundAbscissa(x, hh, xContext, angleUnit);
  T err = max;
  T ans = 0;
  T errt = 0;
  /* Loop on i: change the step size */
  for (int i = 1; i < 10; i++) {
    hh /= k_rateStepSize;
    /* Make hh an exactly representable number */
    volatile T temp =  x+hh;
    hh = temp - x;
    a[0][i] = growthRateAroundAbscissa(x, hh, xContext, angleUnit);
    T fac = k_rateStepSize*k_rateStepSize;
    /* Loop on j: compute extrapolation for several orders */
    for (int j = 1; j < 10; j++) {
      a[j][i] = (a[j-1][i]*fac-a[j-1][i-1])/(fac-1);
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
    if (std::fabs(a[i][i]-a[i-1][i-1]) > 2*err) {
      break;
    }
  }
  /* if the error is too big regarding the value, do not return the answer */
  if (err/ans > k_maxErrorRateOnApproximation || std::isnan(err)) {
    return new Complex<T>(Complex<T>::Float(NAN));
  }
  if (err < min) {
    return new Complex<T>(Complex<T>::Float(ans));
  }
  err = std::pow((T)10, std::floor(std::log10(std::fabs(err)))+2);
  return new Complex<T>(Complex<T>::Float(std::round(ans/err)*err));
}

template<typename T>
T Derivative::growthRateAroundAbscissa(T x, T h, VariableContext<T> xContext, AngleUnit angleUnit) const {
  Symbol xSymbol('x');
  Complex<T> e = Complex<T>::Float(x + h);
  xContext.setExpressionForSymbolName(&e, &xSymbol, xContext);
  Expression * fInput = operand(0)->evaluate<T>(xContext, angleUnit);
  T expressionPlus = fInput->type() == Type::Complex ? static_cast<Complex<T> *>(fInput)->toScalar() : NAN;
  delete fInput;
  e = Complex<T>::Float(x-h);
  xContext.setExpressionForSymbolName(&e, &xSymbol, xContext);
  fInput = operand(0)->evaluate<T>(xContext, angleUnit);
  T expressionMinus = fInput->type() == Type::Complex ? static_cast<Complex<T> *>(fInput)->toScalar() : NAN;
  delete fInput;
  return (expressionPlus - expressionMinus)/(2*h);
}

template<typename T>
T Derivative::approximateDerivate2(T x, T h, VariableContext<T> xContext, AngleUnit angleUnit) const {
  Symbol xSymbol('x');
  Complex<T> e = Complex<T>::Float(x + h);
  xContext.setExpressionForSymbolName(&e, &xSymbol, xContext);
  Expression * fInput = operand(0)->evaluate<T>(xContext, angleUnit);
  T expressionPlus = fInput->type() == Type::Complex ? static_cast<Complex<T> *>(fInput)->toScalar() : NAN;
  delete fInput;
  e = Complex<T>::Float(x);
  xContext.setExpressionForSymbolName(&e, &xSymbol, xContext);
  fInput = operand(0)->evaluate<T>(xContext, angleUnit);
  T expression = fInput->type() == Type::Complex ? static_cast<Complex<T> *>(fInput)->toScalar() : NAN;
  delete fInput;
  e = Complex<T>::Float(x-h);
  xContext.setExpressionForSymbolName(&e, &xSymbol, xContext);
  fInput = operand(0)->evaluate<T>(xContext, angleUnit);
  T expressionMinus = fInput->type() == Type::Complex ? static_cast<Complex<T> *>(fInput)->toScalar() : NAN;
  delete fInput;
  return expressionPlus - 2.0*expression + expressionMinus;
}

}

