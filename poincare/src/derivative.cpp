#include <poincare/derivative.h>
#include <poincare/symbol.h>
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

int Derivative::polynomialDegree(char symbolName) const {
  if (symbolName == 'x') {
    if (operand(1)->polynomialDegree(symbolName) != 0) {
      return -1;
    }
    return 0;
  }
  return Expression::polynomialDegree(symbolName);
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
Complex<T> * Derivative::templatedApproximate(Context& context, AngleUnit angleUnit) const {
  static T min = sizeof(T) == sizeof(double) ? DBL_MIN : FLT_MIN;
  static T epsilon = sizeof(T) == sizeof(double) ? DBL_EPSILON : FLT_EPSILON;
  VariableContext xContext = VariableContext('x', &context);
  Evaluation<T> * xInput = operand(1)->privateApproximate(T(), context, angleUnit);
  T x = xInput->toScalar();
  delete xInput;
  xContext.setApproximationForVariable(x);
  Evaluation<T> * fInput = operand(0)->privateApproximate(T(), xContext, angleUnit);
  T functionValue = fInput->toScalar();
  delete fInput;

  // No complex/matrix version of Derivative
  if (std::isnan(x) || std::isnan(functionValue)) {
    return new Complex<T>(Complex<T>::Undefined());
  }

  T error, result;
  T h = k_minInitialRate;
  do {
    result = riddersApproximation(xContext, angleUnit, x, h, &error);
    h /= 10.0;
  } while ((std::fabs(error/result) > k_maxErrorRateOnApproximation || std::isnan(error)) && h >= epsilon);

  /* if the error is too big regarding the value, do not return the answer */
  if (std::fabs(error/result) > k_maxErrorRateOnApproximation || std::isnan(error)) {
    return new Complex<T>(Complex<T>::Undefined());
  }
  if (std::fabs(error) < min) {
    return new Complex<T>(result);
  }
  error = std::pow((T)10, std::floor(std::log10(std::fabs(error)))+2);
  return new Complex<T>(std::round(result/error)*error);
}

template<typename T>
T Derivative::growthRateAroundAbscissa(T x, T h, VariableContext xContext, AngleUnit angleUnit) const {
  Symbol xSymbol('x');
  xContext.setApproximationForVariable(x + h);
  Evaluation<T> * fInput = operand(0)->privateApproximate(T(), xContext, angleUnit);
  T expressionPlus = fInput->toScalar();
  delete fInput;
  xContext.setApproximationForVariable(x - h);
  fInput = operand(0)->privateApproximate(T(), xContext, angleUnit);
  T expressionMinus = fInput->toScalar();
  delete fInput;
  return (expressionPlus - expressionMinus)/(2*h);
}

template<typename T>
T Derivative::riddersApproximation(VariableContext xContext, AngleUnit angleUnit, T x, T h, T * error) const {
  /* Ridders' Algorithm
   * Blibliography:
   * - Ridders, C.J.F. 1982, Advances in Engineering Software, vol. 4, no. 2,
   * pp. 75–76. */

  *error = sizeof(T) == sizeof(float) ? FLT_MAX : DBL_MAX;
  // Initialize hh
  assert(h != 0.0);
  /* Make hh an exactly representable number */
  volatile T temp =  x+h;
  T hh = temp - x;
  /* a is matrix storing the function extrapolations for different stepsizes at
  * different order */
  T a[10][10];
  for (int i = 0; i < 10; i++) {
    for (int j = 0; j < 10; j++) {
      a[i][j] = 1;
    }
  }
  a[0][0] = growthRateAroundAbscissa(x, hh, xContext, angleUnit);
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
      if (errt < *error) {
        *error = errt;
        ans = a[j][i];
      }
    }
    /* If higher extrapolation order significantly increases the error, return
     * early */
    if (std::fabs(a[i][i]-a[i-1][i-1]) > 2*(*error)) {
      break;
    }
  }
  return ans;
}

}

