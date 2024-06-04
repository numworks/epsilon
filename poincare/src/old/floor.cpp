#include <assert.h>
#include <float.h>
#include <ion.h>
#include <omg/float.h>
#include <poincare/layout.h>
#include <poincare/old/decimal.h>
#include <poincare/old/floor.h>
#include <poincare/old/rational.h>
#include <poincare/old/serialization_helper.h>
#include <poincare/old/simplification_helper.h>
#include <poincare/old/symbol.h>

#include <cmath>

namespace Poincare {

int FloorNode::numberOfChildren() const {
  return Floor::s_functionHelper.numberOfChildren();
}

size_t FloorNode::serialize(char* buffer, size_t bufferSize,
                            Preferences::PrintFloatMode floatDisplayMode,
                            int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(
      this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits,
      Floor::s_functionHelper.aliasesList().mainAlias());
}

OExpression FloorNode::shallowReduce(const ReductionContext& reductionContext) {
  return Floor(this).shallowReduce(reductionContext);
}

OExpression Floor::shallowReduce(ReductionContext reductionContext) {
  {
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
  OExpression c = childAtIndex(0);
  if (c.otype() == ExpressionNode::Type::Rational) {
    Rational r = static_cast<Rational&>(c);
    IntegerDivision div =
        Integer::Division(r.signedIntegerNumerator(), r.integerDenominator());
    assert(!div.quotient.isOverflow());
    OExpression result = Rational::Builder(div.quotient);
    replaceWithInPlace(result);
    return result;
  }
  return shallowReduceUsingApproximation(reductionContext);
}

}  // namespace Poincare
