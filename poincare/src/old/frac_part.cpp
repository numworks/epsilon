#include <poincare/layout.h>
#include <poincare/old/frac_part.h>
#include <poincare/old/rational.h>
#include <poincare/old/serialization_helper.h>
#include <poincare/old/simplification_helper.h>

#include <cmath>

namespace Poincare {

int FracPartNode::numberOfChildren() const {
  return FracPart::s_functionHelper.numberOfChildren();
}

size_t FracPartNode::serialize(char* buffer, size_t bufferSize,
                               Preferences::PrintFloatMode floatDisplayMode,
                               int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(
      this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits,
      FracPart::s_functionHelper.aliasesList().mainAlias());
}

OExpression FracPartNode::shallowReduce(
    const ReductionContext& reductionContext) {
  return FracPart(this).shallowReduce(reductionContext);
}

OExpression FracPart::shallowReduce(ReductionContext reductionContext) {
  {
    OExpression e = SimplificationHelper::defaultShallowReduce(
        *this, &reductionContext,
        SimplificationHelper::BooleanReduction::UndefinedOnBooleans,
        SimplificationHelper::UnitReduction::BanUnits,
        SimplificationHelper::MatrixReduction::UndefinedOnMatrix,
        SimplificationHelper::ListReduction::DistributeOverLists);
    if (!e.isUninitialized()) {
      return e;
    }
  }
  OExpression c = childAtIndex(0);
  if (c.otype() != ExpressionNode::Type::Rational) {
    return *this;
  }
  Rational r = static_cast<Rational&>(c);
  IntegerDivision div =
      Integer::Division(r.signedIntegerNumerator(), r.integerDenominator());
  assert(!div.remainder.isOverflow());
  Integer rDenominator = r.integerDenominator();
  OExpression result = Rational::Builder(div.remainder, rDenominator);
  replaceWithInPlace(result);
  return result;
}

}  // namespace Poincare
