#include <poincare/derivative.h>
#include <poincare/symbol.h>
#include <poincare/simplification_helper.h>
#include <poincare/undefined.h>
#include <cmath>
#include <assert.h>
#include <float.h>

namespace Poincare {

int DerivativeNode::polynomialDegree(char symbolName) const {
  if (childAtIndex(0)->polynomialDegree(symbolName) == 0
      && childAtIndex(1)->polynomialDegree(symbolName) == 0)
  {
    // If no child depends on the symbol, the polynomial degree is 0.
    return 0;
  }
  // If one of the children depends on the symbol, we do not know the degree.
  return ExpressionNode::polynomialDegree(symbolName);
}

Expression DerivativeNode::shallowReduce(Context & context, Preferences::AngleUnit angleUnit) {
  return Derivative(this).shallowReduce(context, angleUnit);
}

template<typename T>
Evaluation<T> DerivativeNode::templatedApproximate(Context& context, Preferences::AngleUnit angleUnit) const {
  static T min = sizeof(T) == sizeof(double) ? DBL_MIN : FLT_MIN;
  static T epsilon = sizeof(T) == sizeof(double) ? DBL_EPSILON : FLT_EPSILON;
  Evaluation<T> xInput = childAtIndex(1)->approximate(T(), context, angleUnit);
  T x = xInput.toScalar();
  T functionValue = Expression(childAtIndex(0)).approximateWithValueForSymbol('x', x, context, angleUnit);
  // No complex/matrix version of Derivative
  if (std::isnan(x) || std::isnan(functionValue)) {
    return Complex<T>::Undefined();
  }

  T error, result;
  T h = k_minInitialRate;
  do {
    result = riddersApproximation(context, angleUnit, x, h, &error);
    h /= 10.0;
  } while ((std::fabs(error/result) > k_maxErrorRateOnApproximation || std::isnan(error)) && h >= epsilon);

  // If the error is too big regarding the value, do not return the answer.
  if (std::fabs(error/result) > k_maxErrorRateOnApproximation || std::isnan(error)) {
    return Complex<T>::Undefined();
  }
  if (std::fabs(error) < min) {
    return Complex<T>(result);
  }
  error = std::pow((T)10, std::floor(std::log10(std::fabs(error)))+2);
  return Complex<T>(std::round(result/error)*error);
}

template<typename T>
T DerivativeNode::growthRateAroundAbscissa(T x, T h, Context & context, Preferences::AngleUnit angleUnit) const {
  T expressionPlus = Expression(childAtIndex(0)).approximateWithValueForSymbol('x', x+h, context, angleUnit);
  T expressionMinus = Expression(childAtIndex(0)).approximateWithValueForSymbol('x', x-h, context, angleUnit);
  return (expressionPlus - expressionMinus)/(2*h);
}

template<typename T>
T DerivativeNode::riddersApproximation(Context & context, Preferences::AngleUnit angleUnit, T x, T h, T * error) const {
  /* Ridders' Algorithm
   * Blibliography:
   * - Ridders, C.J.F. 1982, Advances in Helperering Software, vol. 4, no. 2,
   * pp. 75–76. */

  *error = sizeof(T) == sizeof(float) ? FLT_MAX : DBL_MAX;
  assert(h != 0.0);
  // Initialize hh, make hh an exactly representable number
  volatile T temp = x+h;
  T hh = temp - x;
  /* A is matrix storing the function extrapolations for different stepsizes at
   * different order */
  T a[10][10];
  for (int i = 0; i < 10; i++) {
    for (int j = 0; j < 10; j++) {
      a[i][j] = 1;
    }
  }
  a[0][0] = growthRateAroundAbscissa(x, hh, context, angleUnit);
  T ans = 0;
  T errt = 0;
  // Loop on i: change the step size
  for (int i = 1; i < 10; i++) {
    hh /= k_rateStepSize;
    // Make hh an exactly representable number
    volatile T temp = x+hh;
    hh = temp - x;
    a[0][i] = growthRateAroundAbscissa(x, hh, context, angleUnit);
    T fac = k_rateStepSize*k_rateStepSize;
    // Loop on j: compute extrapolation for several orders
    for (int j = 1; j < 10; j++) {
      a[j][i] = (a[j-1][i]*fac-a[j-1][i-1])/(fac-1);
      fac = k_rateStepSize*k_rateStepSize*fac;
      errt = std::fabs(a[j][i]-a[j-1][i]) > std::fabs(a[j][i]-a[j-1][i-1]) ? std::fabs(a[j][i]-a[j-1][i]) : std::fabs(a[j][i]-a[j-1][i-1]);
      // Update error and answer if error decreases
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

Derivative::Derivative() : Expression(TreePool::sharedPool()->createTreeNode<DerivativeNode>()) {}

Expression Derivative::shallowReduce(Context & context, Preferences::AngleUnit angleUnit) {
  {
    Expression e = Expression::defaultShallowReduce(context, angleUnit);
    if (e.isUndefined()) {
      return e;
    }
  }
#if MATRIX_EXACT_REDUCING
  if (childAtIndex(0).type() == ExpressionNode::Type::Matrix || childAtIndex(1).type() == ExpressionNode::Type::Matrix) {
    return Undefined();
  }
#endif
  // TODO: to be implemented diff(+) -> +diff() etc
  return *this;
}

}

