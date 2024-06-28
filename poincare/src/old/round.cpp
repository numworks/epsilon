#include <assert.h>
#include <poincare/layout.h>
#include <poincare/old/power.h>
#include <poincare/old/rational.h>
#include <poincare/old/round.h>
#include <poincare/old/serialization_helper.h>
#include <poincare/old/simplification_helper.h>
#include <poincare/old/undefined.h>

#include <cmath>
#include <utility>

namespace Poincare {

size_t RoundNode::serialize(char* buffer, size_t bufferSize,
                            Preferences::PrintFloatMode floatDisplayMode,
                            int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(
      this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits,
      Round::s_functionHelper.aliasesList().mainAlias());
}

OExpression RoundNode::shallowReduce(const ReductionContext& reductionContext) {
  return Round(this).shallowReduce(reductionContext);
}

OExpression Round::shallowReduce(ReductionContext reductionContext) {
  {
    if (numberOfChildren() == 2 && childAtIndex(1).hasUnit()) {
      // Number of digits cannot have units
      return replaceWithUndefinedInPlace();
    }
    OExpression e = SimplificationHelper::defaultShallowReduce(
        *this, &reductionContext,
        SimplificationHelper::BooleanReduction::UndefinedOnBooleans,
        SimplificationHelper::UnitReduction::ExtractUnitsOfFirstChild,
        SimplificationHelper::MatrixReduction::UndefinedOnMatrix,
        SimplificationHelper::ListReduction::DistributeOverLists);
    if (!e.isUninitialized()) {
      return e;
    }
  }
  OExpression secondChild;
  if (numberOfChildren() == 1) {
    secondChild = Rational::Builder(0);
  } else {
    secondChild = childAtIndex(1);
  }
  /* We reduce only round(Rational, Rational). We do not reduce
   * round(Float, Float) which is equivalent to what is done in approximate. */
  if (childAtIndex(0).otype() == ExpressionNode::Type::Rational &&
      secondChild.otype() == ExpressionNode::Type::Rational) {
    Rational r1 = childAtIndex(0).convert<Rational>();
    Rational r2 = secondChild.convert<Rational>();
    if (!r2.isInteger()) {
      return replaceWithUndefinedInPlace();
    }
    const Rational ten = Rational::Builder(10);
    if (Power::RationalExponentShouldNotBeReduced(ten, r2)) {
      return *this;
    }
    Rational err = Rational::IntegerPower(ten, r2.signedIntegerNumerator());
    Rational mult = Rational::Multiplication(r1, err);
    if (mult.numeratorOrDenominatorIsInfinity()) {
      return *this;
    }
    IntegerDivision d = Integer::Division(mult.signedIntegerNumerator(),
                                          mult.integerDenominator());
    assert(!d.quotient.isOverflow());
    Integer rounding = d.quotient;
    Integer multDenominator = mult.integerDenominator();
    if (Rational::NaturalOrder(Rational::Builder(d.remainder, multDenominator),
                               Rational::Builder(1, 2)) >= 0) {
      rounding = Integer::Addition(rounding, Integer(1));
    }
    Rational result = Rational::Multiplication(
        Rational::Builder(rounding),
        Rational::IntegerPower(Rational::Builder(1, 10),
                               r2.signedIntegerNumerator()));
    if (result.numeratorOrDenominatorIsInfinity()) {
      return *this;
    }
    replaceWithInPlace(result);
    return std::move(result);
  }
  return *this;
}

}  // namespace Poincare
