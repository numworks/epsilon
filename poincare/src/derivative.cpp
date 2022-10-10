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
      && childAtIndex(2)->polynomialDegree(context, symbolName) == 0
      && childAtIndex(3)->polynomialDegree(context, symbolName) == 0)
  {
    // If no child depends on the symbol, the polynomial degree is 0.
    return 0;
  }
  // If one of the children depends on the symbol, we do not know the degree.
  return ExpressionNode::polynomialDegree(context, symbolName);
}

Layout DerivativeNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits, Context * context) const {
  if (isFirstOrder()) {
    return FirstOrderDerivativeLayout::Builder(
        childAtIndex(0)->createLayout(floatDisplayMode, numberOfSignificantDigits, context),
        childAtIndex(1)->createLayout(floatDisplayMode, numberOfSignificantDigits, context),
        childAtIndex(2)->createLayout(floatDisplayMode, numberOfSignificantDigits, context));
  }
  return HigherOrderDerivativeLayout::Builder(
      childAtIndex(0)->createLayout(floatDisplayMode, numberOfSignificantDigits, context),
      childAtIndex(1)->createLayout(floatDisplayMode, numberOfSignificantDigits, context),
      childAtIndex(2)->createLayout(floatDisplayMode, numberOfSignificantDigits, context),
      childAtIndex(3)->createLayout(floatDisplayMode, numberOfSignificantDigits, context));
}

int DerivativeNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  int lastChildIndex = isFirstOrder() ? numberOfChildren() - 2 : numberOfChildren() - 1;
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, Derivative::s_functionHelper.aliasesList().mainAlias(), SerializationHelper::ParenthesisType::Classic, lastChildIndex);
}

Expression DerivativeNode::shallowReduce(const ReductionContext& reductionContext) {
  return Derivative(this).shallowReduce(reductionContext);
}

template<typename T>
Evaluation<T> DerivativeNode::templatedApproximate(const ApproximationContext& approximationContext) const {
  T orderValue = childAtIndex(numberOfChildren() - 1)->approximate(T(), approximationContext).toScalar();
  if (std::isnan(orderValue) || std::floor(orderValue) != orderValue) {
    return Complex<T>::RealUndefined();
  }
  int order = static_cast<int>(orderValue);
  if (order < 0) {
    return Complex<T>::RealUndefined();
  }
  if (order > k_maxOrderForApproximation) {
    /* FIXME:
     * Since approximation of higher order derivative is exponentially complex,
     * we set a threshold above which we won't compute the derivative.
     *
     * The method we use for now for the higher order derivatives is to
     * recursively approximate the derivatives of lower levels.
     * It's as if we approximated diff(diff(diff(diff(..(diff(f(x)))))))
     * But this is method is way too expensive in time and memory.
     *
     * Other methods exists for approximating higher order derivative.
     * This should be investigated
     * */
    return Complex<T>::RealUndefined();
  }
  T evaluationArgument = childAtIndex(2)->approximate(T(), approximationContext).toScalar();
  // No complex/matrix version of Derivative
  if (std::isnan(evaluationArgument)) {
    return Complex<T>::RealUndefined();
  }
  return Complex<T>::Builder(scalarApproximateWithValueForArgumentAndOrder<T>(evaluationArgument, order, approximationContext));
}


template<typename T>
T DerivativeNode::scalarApproximateWithValueForArgumentAndOrder(T evaluationArgument, int order, const ApproximationContext& approximationContext) const {
  /* TODO : Reduction is mapped on list, but not approximation.
  * Find a smart way of doing it. */
  assert(order >= 0);
  if (order == 0) {
    return firstChildScalarValueForArgument(evaluationArgument, approximationContext);
  }
  T functionValue = scalarApproximateWithValueForArgumentAndOrder(evaluationArgument, order - 1, approximationContext);
  if (std::isnan(functionValue)) {
    return NAN;
  }

  T error = sizeof(T) == sizeof(double) ? DBL_MAX : FLT_MAX;
  T result = 1.0;
  T h = k_minInitialRate;
  constexpr T tenEpsilon = static_cast<T>(10.0)*Float<T>::Epsilon();
  do {
    T currentError;
    T currentResult = riddersApproximation(order, approximationContext, evaluationArgument, h, &currentError);
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
    return NAN;
  }
  // Round and amplify error to a power of 10
  T roundedError = static_cast<T>(100.0) * std::pow(static_cast<T>(10.0), IEEE754<T>::exponentBase10(error));
  if (error == static_cast<T>(0.0) || std::round(result/roundedError) == result/roundedError) {
    // Return result if error is negligible
    return result;
  }
  /* Round down the result, to remove precision depending on roundedError. The
   * higher the error is to the result, the lesser the output will have
   * significant numbers.
   * - if result  >> roundedError , almost no loss of precision
   * - if result  ~= error, precision reduced to 1 significant number
   * - if result*2 < error, 0 is returned  */
  return std::round(result/roundedError)*roundedError;
}

template<typename T>
T DerivativeNode::growthRateAroundAbscissa(T x, T h, int order, const ApproximationContext& approximationContext) const {
  T expressionPlus = scalarApproximateWithValueForArgumentAndOrder(x+h, order - 1, approximationContext);
  T expressionMinus = scalarApproximateWithValueForArgumentAndOrder(x-h, order - 1, approximationContext);
  return (expressionPlus - expressionMinus)/(h+h);
}

template<typename T>
T DerivativeNode::riddersApproximation(int order, const ApproximationContext& approximationContext, T x, T h, T * error) const {
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
  a[0][0] = growthRateAroundAbscissa(x, hh, order, approximationContext);
  T ans = 0;
  T errt = 0;
  // Loop on i: change the step size
  for (int i = 1; i < 10; i++) {
    hh /= k_rateStepSize;
    // Make hh an exactly representable number
    volatile T temp = x+hh;
    hh = temp - x;
    a[0][i] = growthRateAroundAbscissa(x, hh, order, approximationContext);
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

void Derivative::deepReduceChildren(const ExpressionNode::ReductionContext& reductionContext) {
  /* First child is reduced with target SystemForAnalysis */
  ExpressionNode::ReductionContext childContext = reductionContext;
  childContext.setTarget(ExpressionNode::ReductionTarget::SystemForAnalysis);
  childAtIndex(0).deepReduce(childContext);

  /* Other children are reduced with the same reduction target as the parent */
  const int childrenCount = numberOfChildren();
  assert(childrenCount > 1);
  for (int i = 1; i < childrenCount; i++) {
    childAtIndex(i).deepReduce(reductionContext);
  }
}

Expression Derivative::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
  {
    Expression e = SimplificationHelper::defaultShallowReduce(
        *this,
        &reductionContext,
        SimplificationHelper::BooleanReduction::UndefinedOnBooleans,
        SimplificationHelper::UnitReduction::BanUnits,
        SimplificationHelper::MatrixReduction::UndefinedOnMatrix,
        SimplificationHelper::ListReduction::DistributeOverLists
    );
    if (!e.isUninitialized()) {
      return e;
    }
  }

  int derivationOrder;
  bool orderIsSymbol;
  bool orderIsInteger = SimplificationHelper::extractIntegerChildAtIndex(*this, numberOfChildren() - 1, &derivationOrder, &orderIsSymbol);
  if (orderIsSymbol) {
    return *this;
  }
  if (!orderIsInteger || derivationOrder < 0) {
    return replaceWithUndefinedInPlace();
  }

  List listOfDependencies = List::Builder();
  Expression derivand = childAtIndex(0);
  if (derivand.type() == ExpressionNode::Type::Dependency) {
    static_cast<Dependency &>(derivand).extractDependencies(listOfDependencies);
    derivand = childAtIndex(0);
  }
  Symbol symbol = childAtIndex(1).convert<Symbol>();
  Expression symbolValue = childAtIndex(2);
  Expression derivandAsDependency = derivand.clone();

  int currentDerivationOrder = derivationOrder;
  /* Since derivand is a child to the derivative node, it can be replaced in
   * place without derivate having to return the derivative. */
  while (currentDerivationOrder > 0 && derivand.derivate(reductionContext, symbol, symbolValue)) {
    /* We need to reduce the derivand here before replacing the symbol: the
     * general formulas used during the derivation process can create some nodes
     * that are not defined for some values (e.g. log), but that would disappear
     * at reduction. */
    Expression reducedDerivative = childAtIndex(0).deepReduce(reductionContext);
    if (reducedDerivative.type() == ExpressionNode::Type::Dependency) {
      /* Remove the dependencies created during derivation reduction since they
       * were just generated by the calculation process but do not reflect
       * the real definition domain of the derivative.
       * Example: 3x is derivated to 0*x+3*1 because we use the u'v+uv' formula.
       * So a dependency is created on x (because of 0*x), which makes no sense.*/
      reducedDerivative = reducedDerivative.childAtIndex(0);
    }
    replaceChildAtIndexInPlace(0, reducedDerivative);
    if (Expression::IsZero(reducedDerivative)) {
      currentDerivationOrder = 0;
    } else {
      currentDerivationOrder--;
    }
    /* Updates the value of derivand, because derivate may call
     * replaceWithInplace on it. */
    derivand = childAtIndex(0);
  }
  if (currentDerivationOrder < derivationOrder) {
    // Do not add a dependecy if nothing was derivated.
    listOfDependencies.addChildAtIndexInPlace(derivandAsDependency, listOfDependencies.numberOfChildren(), listOfDependencies.numberOfChildren());
  }
  if (currentDerivationOrder == 0) {
    /* Deep reduces the child, because derivate may not preserve its reduced
     * status. */
    Dependency d = Dependency::Builder(childAtIndex(0), listOfDependencies);
    Expression result = d.replaceSymbolWithExpression(symbol, symbolValue);
    replaceWithInPlace(result);
    return result.deepReduce(reductionContext);
  }

  // If derivation fails, we still need to decrease the derivation order according to what was already done.
  replaceChildAtIndexInPlace(numberOfChildren() - 1, Rational::Builder(currentDerivationOrder));
  Dependency d = Dependency::Builder(Undefined::Builder(), listOfDependencies);
  replaceWithInPlace(d);
  d.replaceChildAtIndexInPlace(0, *this);
  return d.shallowReduce(reductionContext);
}

void Derivative::DerivateUnaryFunction(Expression function, Symbol symbol, Expression symbolValue, const ExpressionNode::ReductionContext& reductionContext) {
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
  if (children.numberOfChildren() < 4) {
    return Builder(children.childAtIndex(0), children.childAtIndex(1).convert<Symbol>(), children.childAtIndex(2));
  } else {
    return Builder(children.childAtIndex(0), children.childAtIndex(1).convert<Symbol>(), children.childAtIndex(2), children.childAtIndex(3));
  }
}

}