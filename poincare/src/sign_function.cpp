#include <assert.h>
#include <ion.h>
#include <poincare/complex_cartesian.h>
#include <poincare/derivative.h>
#include <poincare/float.h>
#include <poincare/layout_helper.h>
#include <poincare/multiplication.h>
#include <poincare/rational.h>
#include <poincare/serialization_helper.h>
#include <poincare/sign_function.h>
#include <poincare/simplification_helper.h>
#include <poincare/symbol.h>
#include <poincare/symbol_abstract.h>
#include <poincare/undefined.h>

#include <cmath>
#include <utility>

#include "poincare/comparison.h"
#include "poincare/piecewise_operator.h"

namespace Poincare {

int SignFunctionNode::numberOfChildren() const {
  return SignFunction::s_functionHelper.numberOfChildren();
}

size_t SignFunctionNode::serialize(char* buffer, size_t bufferSize,
                                   Preferences::PrintFloatMode floatDisplayMode,
                                   int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(
      this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits,
      SignFunction::s_functionHelper.aliasesList().mainAlias());
}

Expression SignFunctionNode::shallowReduce(
    const ReductionContext& reductionContext) {
  return SignFunction(this).shallowReduce(reductionContext);
}

bool SignFunctionNode::derivate(const ReductionContext& reductionContext,
                                Symbol symbol, Expression symbolValue) {
  return SignFunction(this).derivate(reductionContext, symbol, symbolValue);
}

template <typename T>
std::complex<T> SignFunctionNode::computeOnComplex(
    const std::complex<T> c, Preferences::ComplexFormat,
    Preferences::AngleUnit angleUnit) {
  if (c.imag() != 0 || std::isnan(c.real())) {
    return complexRealNAN<T>();
  }
  if (c.real() == 0) {
    return 0.0;
  }
  if (c.real() < 0) {
    return -1.0;
  }
  return 1.0;
}

Expression SignFunction::shallowReduce(ReductionContext reductionContext) {
  {
    Expression e = SimplificationHelper::defaultShallowReduce(
        *this, &reductionContext,
        SimplificationHelper::BooleanReduction::UndefinedOnBooleans,
        SimplificationHelper::UnitReduction::KeepUnits,
        SimplificationHelper::MatrixReduction::UndefinedOnMatrix,
        SimplificationHelper::ListReduction::DistributeOverLists);
    if (!e.isUninitialized()) {
      return e;
    }
    // Discard units if any
    Expression unit;
    childAtIndex(0).removeUnit(&unit);
  }
  Expression child = childAtIndex(0);
  Expression resultSign;
  TrinaryBoolean childIsPositive = child.isPositive(reductionContext.context());
  TrinaryBoolean childIsNull = child.isNull(reductionContext.context());
  if (childIsPositive != TrinaryBoolean::Unknown &&
      (childIsNull != TrinaryBoolean::Unknown ||
       reductionContext.target() == ReductionTarget::User)) {
    // If target == User, we want sign(abs(x)) = 1, even if x can be null.
    resultSign = Rational::Builder(
        childIsNull == TrinaryBoolean::True
            ? 0
            : (childIsPositive == TrinaryBoolean::True ? 1 : -1));
  } else {
    Evaluation<float> childApproximated = child.node()->approximate(
        1.0f, ApproximationContext(reductionContext, true));
    if (childApproximated.type() != EvaluationNode<float>::Type::Complex) {
      return replaceWithUndefinedInPlace();
    }
    Complex<float> c = static_cast<Complex<float>&>(childApproximated);
    if (std::isnan(c.imag()) || std::isnan(c.real()) || c.imag() != 0) {
      /* c's approximation has no sign (c is complex or NAN)
       * sign(-x) = -sign(x) */
      Expression oppChild =
          child.makePositiveAnyNegativeNumeralFactor(reductionContext);
      if (oppChild.isUninitialized()) {
        return *this;
      }
      Expression sign = *this;
      Multiplication m = Multiplication::Builder(Rational::Builder(-1));
      replaceWithInPlace(m);
      m.addChildAtIndexInPlace(sign, 1, 1);
      /* sign doesn't need to be shallowReduced because x = -NAN --> x = NAN
       * m doesn't need to be shallowReduced because -1*sign cannot be reduced*/
      return std::move(m);
    }
    resultSign = Rational::Builder(c.real() > 0 ? 1 : (c.real() < 0 ? -1 : 0));
  }
  replaceWithInPlace(resultSign);
  return resultSign;
}

bool SignFunction::derivate(const ReductionContext& reductionContext,
                            Symbol symbol, Expression symbolValue) {
  {
    Expression e = Derivative::DefaultDerivate(*this, reductionContext, symbol);
    if (!e.isUninitialized()) {
      return true;
    }
  }

  /* This function derivate is equal to 0 everywhere but in 0 where
   * it's not defined.
   * We use a piecewise function instead of a dependency as derivate will strip
   * all dependencies that arise during the derivation process.
   * This derivative is used in the derivative of arccot(x) and abs(x).
   */
  Comparison condition = Comparison::Builder(
      childAtIndex(0), ComparisonNode::OperatorType::NotEqual,
      Rational::Builder(0));
  List arguments = List::Builder();
  arguments.addChildAtIndexInPlace(Rational::Builder(0), 0, 0);
  arguments.addChildAtIndexInPlace(condition, 1, 1);
  replaceWithInPlace(PiecewiseOperator::UntypedBuilder(arguments));
  return true;
}

}  // namespace Poincare
