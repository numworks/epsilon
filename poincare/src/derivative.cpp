#include <poincare/derivative.h>
#include <poincare/ieee754.h>
#include <poincare/layout_helper.h>
#include <poincare/serialization_helper.h>

#include <poincare/symbol.h>
#include <poincare/undefined.h>
#include <cmath>
#include <assert.h>
#include <float.h>

namespace Poincare {

constexpr Expression::FunctionHelper Derivative::s_functionHelper;

int DerivativeNode::numberOfChildren() const { return Derivative::s_functionHelper.numberOfChildren(); }

int DerivativeNode::polynomialDegree(Context * context, const char * symbolName) const {
  if (childAtIndex(0)->polynomialDegree(context, symbolName) == 0
      && childAtIndex(1)->polynomialDegree(context, symbolName) == 0
      && childAtIndex(2)->polynomialDegree(context, symbolName) == 0)
  {
    // If no child depends on the symbol, the polynomial degree is 0.
    return 0;
  }
  // If one of the children depends on the symbol, we do not know the degree.
  return ExpressionNode::polynomialDegree(context, symbolName);
}

Layout DerivativeNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutHelper::Prefix(this, floatDisplayMode, numberOfSignificantDigits, Derivative::s_functionHelper.name());
}

int DerivativeNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, Derivative::s_functionHelper.name());
}

Expression DerivativeNode::shallowReduce(ReductionContext reductionContext) {
  return Derivative(this).shallowReduce(reductionContext.context());
}

template<typename T>
Evaluation<T> DerivativeNode::templatedApproximate(Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const {
  Evaluation<T> evaluationArgumentInput = childAtIndex(2)->approximate(T(), context, complexFormat, angleUnit);
  T evaluationArgument = evaluationArgumentInput.toScalar();
  T functionValue = approximateWithArgument(evaluationArgument, context, complexFormat, angleUnit);
  // No complex/matrix version of Derivative
  if (std::isnan(evaluationArgument) || std::isnan(functionValue)) {
    return Complex<T>::RealUndefined();
  }

  T error = sizeof(T) == sizeof(double) ? DBL_MAX : FLT_MAX;
  T result;
  T h = k_minInitialRate;
  static T tenEpsilon = sizeof(T) == sizeof(double) ? 10.0*DBL_EPSILON : 10.0f*FLT_EPSILON;
  do {
    T currentError;
    T currentResult = riddersApproximation(context, complexFormat, angleUnit, evaluationArgument, h, &currentError);
    h /= (T)10.0;
    if (std::isnan(currentError) || currentError > error) {
      continue;
    }
    error = currentError;
    result = currentResult;
  } while ((std::fabs(error/result) > k_maxErrorRateOnApproximation || std::isnan(error)) && h >= tenEpsilon);

  /* If the error is too big regarding the value, do not return the answer.
   * If the result is close to 0, we relax this constraint because the error
   * will tend to be bigger compared to the result. */
  if (std::isnan(error)
      || (std::fabs(result) < k_maxErrorRateOnApproximation && std::fabs(error) > std::fabs(result))
      || (std::fabs(result) >= k_maxErrorRateOnApproximation && std::fabs(error/result) > k_maxErrorRateOnApproximation))
  {
    return Complex<T>::RealUndefined();
  }
  static T min = sizeof(T) == sizeof(double) ? DBL_MIN : FLT_MIN;
  if (std::fabs(error) < min) {
    return Complex<T>::Builder(result);
  }
  // Round the result according to the error
  error = std::pow((T)10, IEEE754<T>::exponentBase10(error)+2);
  return Complex<T>::Builder(std::round(result/error)*error);
}

template<typename T>
T DerivativeNode::approximateWithArgument(T x, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const {
  assert(childAtIndex(1)->type() == Type::Symbol);
  VariableContext variableContext = VariableContext(static_cast<SymbolNode *>(childAtIndex(1))->name(), context);
  variableContext.setApproximationForVariable<T>(x);
  // Here we cannot use Expression::approximateWithValueForSymbol which would reset the sApproximationEncounteredComplex flag
  return childAtIndex(0)->approximate(T(), &variableContext, complexFormat, angleUnit).toScalar();
}

template<typename T>
T DerivativeNode::growthRateAroundAbscissa(T x, T h, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const {
  T expressionPlus = approximateWithArgument(x+h, context, complexFormat, angleUnit);
  T expressionMinus = approximateWithArgument(x-h, context, complexFormat, angleUnit);
  return (expressionPlus - expressionMinus)/(h+h);
}

template<typename T>
T DerivativeNode::riddersApproximation(Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, T x, T h, T * error) const {
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
  a[0][0] = growthRateAroundAbscissa(x, hh, context, complexFormat, angleUnit);
  T ans = 0;
  T errt = 0;
  // Loop on i: change the step size
  for (int i = 1; i < 10; i++) {
    hh /= k_rateStepSize;
    // Make hh an exactly representable number
    volatile T temp = x+hh;
    hh = temp - x;
    a[0][i] = growthRateAroundAbscissa(x, hh, context, complexFormat, angleUnit);
    T fac = k_rateStepSize*k_rateStepSize;
    // Loop on j: compute extrapolation for several orders
    for (int j = 1; j < 10; j++) {
      a[j][i] = (a[j-1][i]*fac-a[j-1][i-1])/(fac-1);
      fac = k_rateStepSize*k_rateStepSize*fac;
      T err1 = std::fabs(a[j][i]-a[j-1][i]);
      T err2 = std::fabs(a[j][i]-a[j-1][i-1]);
      errt = err1 > err2 ? err1 : err2;
      // Update error and answer if error decreases
      if (errt < *error) {
        *error = errt;
        ans = a[j][i];
      }
    }
    /* If higher extrapolation order significantly increases the error, return
     * early */
    if (std::fabs(a[i][i]-a[i-1][i-1]) > (*error) + (*error)) {
      break;
    }
  }
  return ans;
}

Expression Derivative::shallowReduce(Context * context) {
  {
    Expression e = Expression::defaultShallowReduce();
    e = e.defaultHandleUnitsInChildren();
    if (e.isUndefined()) {
      return e;
    }
  }
  assert(!childAtIndex(1).deepIsMatrix(context));
  if (childAtIndex(0).deepIsMatrix(context) || childAtIndex(2).deepIsMatrix(context)) {
    return replaceWithUndefinedInPlace();
  }
  // TODO: to be implemented diff(+) -> +diff() etc
  return *this;
}

Expression Derivative::UntypedBuilder(Expression children) {
  assert(children.type() == ExpressionNode::Type::Matrix);
  if (children.childAtIndex(1).type() != ExpressionNode::Type::Symbol) {
    // Second parameter must be a Symbol.
    return Expression();
  }
  return Builder(children.childAtIndex(0), children.childAtIndex(1).convert<Symbol>(), children.childAtIndex(2));
}

}
