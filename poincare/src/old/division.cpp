#include <assert.h>
#include <float.h>
#include <poincare/layout.h>
#include <poincare/old/addition.h>
#include <poincare/old/division.h>
#include <poincare/old/multiplication.h>
#include <poincare/old/opposite.h>
#include <poincare/old/power.h>
#include <poincare/old/rational.h>
#include <poincare/old/serialization_helper.h>
#include <poincare/old/simplification_helper.h>
#include <poincare/old/subtraction.h>
#include <string.h>

#include <cmath>

namespace Poincare {

OMG::Troolean DivisionNode::isPositive(Context *context) const {
  OMG::Troolean numeratorPositive = childAtIndex(0)->isPositive(context);
  OMG::Troolean denominatorPositive = childAtIndex(1)->isPositive(context);
  if (numeratorPositive == OMG::Troolean::Unknown ||
      denominatorPositive == OMG::Troolean::Unknown) {
    return OMG::Troolean::Unknown;
  }
  return OMG::BoolToTroolean(numeratorPositive == denominatorPositive);
}

bool DivisionNode::childNeedsSystemParenthesesAtSerialization(
    const PoolObject *child) const {
  if (static_cast<const ExpressionNode *>(child)->isNumber() &&
      Number(static_cast<const NumberNode *>(child)).isPositive() ==
          OMG::Troolean::False) {
    return true;
  }
  if (static_cast<const ExpressionNode *>(child)->otype() == Type::Rational &&
      !static_cast<const RationalNode *>(child)->isInteger()) {
    return true;
  }
  return static_cast<const ExpressionNode *>(child)->isOfType(
      {Type::Subtraction, Type::Opposite, Type::Multiplication, Type::Division,
       Type::Addition, Type::PercentSimple, Type::PercentAddition,
       Type::MixedFraction, Type::Comparison, Type::LogicalOperatorNot,
       Type::BinaryLogicalOperator});
}

size_t DivisionNode::serialize(char *buffer, size_t bufferSize,
                               Preferences::PrintFloatMode floatDisplayMode,
                               int numberOfSignificantDigits) const {
  return SerializationHelper::Infix(this, buffer, bufferSize, floatDisplayMode,
                                    numberOfSignificantDigits, "/");
}

OExpression DivisionNode::shallowReduce(
    const ReductionContext &reductionContext) {
  return Division(this).shallowReduce(reductionContext);
}

// Division
OExpression Division::shallowReduce(ReductionContext reductionContext) {
  {
    OExpression e = SimplificationHelper::defaultShallowReduce(
        *this, &reductionContext,
        SimplificationHelper::BooleanReduction::UndefinedOnBooleans);
    if (!e.isUninitialized()) {
      return e;
    }
  }
  /* For matrices: we decided that A/B is undefined, as it is a non-standard
   * shortcut */
  if (childAtIndex(1).deepIsMatrix(reductionContext.context(),
                                   reductionContext.shouldCheckMatrices())) {
    return replaceWithUndefinedInPlace();
  }
  OExpression p = Power::Builder(childAtIndex(1), Rational::Builder(-1));
  Multiplication m = Multiplication::Builder(childAtIndex(0), p);
  /* For instance: Division::Builder(2,1). p would be 1^(-1) which can be
   * simplified */
  p.shallowReduce(reductionContext);
  replaceWithInPlace(m);
  return m.shallowReduce(reductionContext);
}

}  // namespace Poincare
