#include <assert.h>
#include <ion.h>
#include <omg/float.h>
#include <poincare/layout.h>
#include <poincare/old/comparison.h>
#include <poincare/old/complex_cartesian.h>
#include <poincare/old/derivative.h>
#include <poincare/old/multiplication.h>
#include <poincare/old/piecewise_operator.h>
#include <poincare/old/rational.h>
#include <poincare/old/serialization_helper.h>
#include <poincare/old/sign_function.h>
#include <poincare/old/simplification_helper.h>
#include <poincare/old/symbol.h>
#include <poincare/old/symbol_abstract.h>
#include <poincare/old/undefined.h>

#include <cmath>
#include <utility>

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

OExpression SignFunctionNode::shallowReduce(
    const ReductionContext& reductionContext) {
  return SignFunction(this).shallowReduce(reductionContext);
}

bool SignFunctionNode::derivate(const ReductionContext& reductionContext,
                                Symbol symbol, OExpression symbolValue) {
  return SignFunction(this).derivate(reductionContext, symbol, symbolValue);
}

OExpression SignFunction::shallowReduce(ReductionContext reductionContext) {
  {
    OExpression e = SimplificationHelper::defaultShallowReduce(
        *this, &reductionContext,
        SimplificationHelper::BooleanReduction::UndefinedOnBooleans,
        SimplificationHelper::UnitReduction::KeepUnits,
        SimplificationHelper::MatrixReduction::UndefinedOnMatrix,
        SimplificationHelper::ListReduction::DistributeOverLists);
    if (!e.isUninitialized()) {
      return e;
    }
    // Discard units if any
    OExpression unit;
    childAtIndex(0).removeUnit(&unit);
  }
  OExpression child = childAtIndex(0);
  OExpression resultSign;
  OMG::Troolean childIsPositive = child.isPositive(reductionContext.context());
  OMG::Troolean childIsNull = child.isNull(reductionContext.context());
  if (childIsPositive != OMG::Troolean::Unknown &&
      (childIsNull != OMG::Troolean::Unknown ||
       reductionContext.target() == ReductionTarget::User)) {
    // If target == User, we want sign(abs(x)) = 1, even if x can be null.
    resultSign = Rational::Builder(
        childIsNull == OMG::Troolean::True
            ? 0
            : (childIsPositive == OMG::Troolean::True ? 1 : -1));
  } else {
    Evaluation<float> childApproximated = child.node()->approximate(
        1.0f, ApproximationContext(reductionContext, true));
    if (childApproximated.otype() != EvaluationNode<float>::Type::Complex) {
      return replaceWithUndefinedInPlace();
    }
    Complex<float> c = static_cast<Complex<float>&>(childApproximated);
    if (std::isnan(c.imag()) || std::isnan(c.real()) || c.imag() != 0) {
      /* c's approximation has no sign (c is complex or NAN)
       * sign(-x) = -sign(x) */
      OExpression oppChild =
          child.makePositiveAnyNegativeNumeralFactor(reductionContext);
      if (oppChild.isUninitialized()) {
        return *this;
      }
      OExpression sign = *this;
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
                            Symbol symbol, OExpression symbolValue) {
  {
    OExpression e =
        Derivative::DefaultDerivate(*this, reductionContext, symbol);
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
  OList arguments = OList::Builder();
  arguments.addChildAtIndexInPlace(Rational::Builder(0), 0, 0);
  arguments.addChildAtIndexInPlace(condition, 1, 1);
  replaceWithInPlace(PiecewiseOperator::UntypedBuilder(arguments));
  return true;
}

}  // namespace Poincare
