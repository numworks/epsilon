#include <poincare/derivative.h>
#include <poincare/dependency.h>
#include <poincare/derivative_layout.h>
#include <poincare/float.h>
#include <poincare/ieee754.h>
#include <poincare/layout_helper.h>
#include <poincare/multiplication.h>
#include <poincare/serialization_helper.h>
#include <poincare/simplification_helper.h>
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
  return DerivativeLayout::Builder(
      childAtIndex(0)->createLayout(floatDisplayMode, numberOfSignificantDigits),
      childAtIndex(1)->createLayout(floatDisplayMode, numberOfSignificantDigits),
      childAtIndex(2)->createLayout(floatDisplayMode, numberOfSignificantDigits));
}

int DerivativeNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, Derivative::s_functionHelper.name());
}

Expression DerivativeNode::shallowReduce(ReductionContext reductionContext) {
  return Derivative(this).shallowReduce(reductionContext);
}

template<typename T>
Evaluation<T> DerivativeNode::templatedApproximate(ApproximationContext approximationContext) const {
  /* TODO : Reduction is mapped on list, but not approximation.
  * Find a smart way of doing it. */
  Evaluation<T> evaluationArgumentInput = childAtIndex(2)->approximate(T(), approximationContext);
  T evaluationArgument = evaluationArgumentInput.toScalar();
  T functionValue = firstChildScalarValueForArgument(evaluationArgument, approximationContext);
  // No complex/matrix version of Derivative
  if (std::isnan(evaluationArgument) || std::isnan(functionValue)) {
    return Complex<T>::RealUndefined();
  }

  T error = sizeof(T) == sizeof(double) ? DBL_MAX : FLT_MAX;
  T result = 1.0;
  T h = k_minInitialRate;
  constexpr T tenEpsilon = static_cast<T>(10.0)*Float<T>::Epsilon();
  do {
    T currentError;
    T currentResult = riddersApproximation(approximationContext, evaluationArgument, h, &currentError);
    h /= static_cast<T>(10.0);
    if (std::isnan(currentError) || currentError > error) {
      continue;
    }
    error = currentError;
    result = currentResult;
  } while ((std::fabs(error/result) > k_maxErrorRateOnApproximation || std::isnan(error)) && h >= tenEpsilon);

  /* Result is discarded if error is both higher than k_minSignificantError and
   * k_maxErrorRateOnApproximation * result. For example, (error, result) can
   * reach (2e-11, 3e-11) or (1e-12, 2e-14) for expected 0 results, with floats
   * as well as with doubles. */
  if (std::isnan(error) || (std::fabs(error) > k_minSignificantError && std::fabs(error) > std::fabs(result) * k_maxErrorRateOnApproximation)) {
    return Complex<T>::RealUndefined();
  }
  // Round and amplify error to a power of 10
  T roundedError = static_cast<T>(100.0) * std::pow(static_cast<T>(10.0), IEEE754<T>::exponentBase10(error));
  if (error == static_cast<T>(0.0) || std::round(result/roundedError) == result/roundedError) {
    // Return result if error is negligible
    return Complex<T>::Builder(result);
  }
  /* Round down the result, to remove precision depending on roundedError. The
   * higher the error is to the result, the lesser the output will have
   * significant numbers.
   * - if result  >> roundedError , almost no loss of precision
   * - if result  ~= error, precision reduced to 1 significant number
   * - if result*2 < error, 0 is returned  */
  return Complex<T>::Builder(std::round(result/roundedError)*roundedError);
}

template<typename T>
T DerivativeNode::growthRateAroundAbscissa(T x, T h, ApproximationContext approximationContext) const {
  T expressionPlus = firstChildScalarValueForArgument(x+h, approximationContext);
  T expressionMinus = firstChildScalarValueForArgument(x-h, approximationContext);
  return (expressionPlus - expressionMinus)/(h+h);
}

template<typename T>
T DerivativeNode::riddersApproximation(ApproximationContext approximationContext, T x, T h, T * error) const {
  /* Ridders' Algorithm
   * Blibliography:
   * - Ridders, C.J.F. 1982, Advances in Helperering Software, vol. 4, no. 2,
   * pp. 75–76. */

  *error = sizeof(T) == sizeof(float) ? FLT_MAX : DBL_MAX;
  assert(h != 0.0);
  // Initialize hh, make hh an exactly representable number
  volatile T temp = x+h;
  T hh = temp - x;
  /* A is matrix storing the function extrapolations for different step sizes at
   * different order */
  T a[10][10];
  for (int i = 0; i < 10; i++) {
    for (int j = 0; j < 10; j++) {
      a[i][j] = 1;
    }
  }
  a[0][0] = growthRateAroundAbscissa(x, hh, approximationContext);
  T ans = 0;
  T errt = 0;
  // Loop on i: change the step size
  for (int i = 1; i < 10; i++) {
    hh /= k_rateStepSize;
    // Make hh an exactly representable number
    volatile T temp = x+hh;
    hh = temp - x;
    a[0][i] = growthRateAroundAbscissa(x, hh, approximationContext);
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

Expression Derivative::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
  {
    Expression e = SimplificationHelper::defaultShallowReduce(*this);
    if (!e.isUninitialized()) {
      return e;
    }
    e = SimplificationHelper::undefinedOnMatrix(*this, reductionContext);
    if (!e.isUninitialized()) {
      return e;
    }
    e = SimplificationHelper::distributeReductionOverLists(*this, reductionContext);
    if (!e.isUninitialized()) {
      return e;
    }
  }
  List l = List::Builder();
  Expression derivand = childAtIndex(0);
  if (derivand.type() == ExpressionNode::Type::Dependency) {
    static_cast<Dependency &>(derivand).extractDependencies(l);
    derivand = childAtIndex(0);
  }
  Symbol symbol = childAtIndex(1).convert<Symbol>();
  Expression symbolValue = childAtIndex(2);
  Expression derivandAsDependency = derivand.clone();

  /* Since derivand is a child to the derivative node, it can be replaced in
   * place without derivate having to return the derivative. */
  if (!derivand.derivate(reductionContext, symbol, symbolValue)) {
    if (l.numberOfChildren() > 0) {
      replaceChildAtIndexInPlace(0, Dependency::Builder(derivand, l));
    }
    return *this;
  }
  /* Updates the value of derivand, because derivate may call
   * replaceWithInplace on it.
   * We need to reduce the derivand here before replacing the symbol : the
   * general formulas used during the derivation process can create some nodes
   * that are not defined for some values (e.g. log), but that would disappear
   * at reduction. */
  Dependency d = Dependency::Builder(childAtIndex(0).deepReduce(reductionContext), l);
  d.addDependency(derivandAsDependency);

  /* Deep reduces the child, because derivate may not preserve its reduced
   * status. */
  Expression result = d.replaceSymbolWithExpression(symbol, symbolValue);
  replaceWithInPlace(result);
  return result.deepReduce(reductionContext);
}

void Derivative::DerivateUnaryFunction(Expression function, Symbol symbol, Expression symbolValue, ExpressionNode::ReductionContext reductionContext) {
  Expression df = function.unaryFunctionDifferential(reductionContext);
  Expression g = function.childAtIndex(0);
  Expression dg = g.derivate(reductionContext, symbol, symbolValue) ? function.childAtIndex(0) : Derivative::Builder(function.childAtIndex(0), symbol.clone().convert<Symbol>(), symbolValue.clone());
  function.replaceWithInPlace(Multiplication::Builder(df, dg));

}

Expression Derivative::UntypedBuilder(Expression children) {
  assert(children.type() == ExpressionNode::Type::List);
  if (children.childAtIndex(1).type() != ExpressionNode::Type::Symbol) {
    // Second parameter must be a Symbol.
    return Expression();
  }
  return Builder(children.childAtIndex(0), children.childAtIndex(1).convert<Symbol>(), children.childAtIndex(2));
}

}
