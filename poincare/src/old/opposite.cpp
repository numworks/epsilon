#include <assert.h>
#include <poincare/layout.h>
#include <poincare/old/addition.h>
#include <poincare/old/constant.h>
#include <poincare/old/multiplication.h>
#include <poincare/old/opposite.h>
#include <poincare/old/rational.h>
#include <poincare/old/serialization_helper.h>
#include <poincare/old/simplification_helper.h>
#include <stdlib.h>

#include <cmath>
#include <utility>

namespace Poincare {

/* Layout */

bool OppositeNode::childAtIndexNeedsUserParentheses(const OExpression& child,
                                                    int childIndex) const {
  assert(childIndex == 0);
  if (child.isNumber() &&
      static_cast<const Number&>(child).isPositive() == OMG::Troolean::False) {
    return true;
  }
  if (child.isOfType({Type::Conjugate, Type::Dependency})) {
    return childAtIndexNeedsUserParentheses(child.childAtIndex(0), childIndex);
  }
  return child.isOfType({Type::Addition, Type::Subtraction, Type::Opposite});
}

size_t OppositeNode::serialize(char* buffer, size_t bufferSize,
                               Preferences::PrintFloatMode floatDisplayMode,
                               int numberOfSignificantDigits) const {
  if (bufferSize == 0) {
    return bufferSize - 1;
  }
  buffer[bufferSize - 1] = 0;
  if (bufferSize == 1) {
    return bufferSize - 1;
  }
  size_t numberOfChar = SerializationHelper::CodePoint(buffer, bufferSize, '-');
  if (numberOfChar >= bufferSize - 1) {
    return bufferSize - 1;
  }
  numberOfChar += childAtIndex(0)->serialize(
      buffer + numberOfChar, bufferSize - numberOfChar, floatDisplayMode,
      numberOfSignificantDigits);
  return numberOfChar;
}

OExpression OppositeNode::shallowReduce(
    const ReductionContext& reductionContext) {
  return Opposite(this).shallowReduce(reductionContext);
}

/* Simplification */

OExpression Opposite::shallowReduce(ReductionContext reductionContext) {
  OExpression result =
      SimplificationHelper::defaultShallowReduce(*this, &reductionContext);
  if (!result.isUninitialized()) {
    return result;
  }
  OExpression child = childAtIndex(0);
  result = Multiplication::Builder(Rational::Builder(-1), child);
  replaceWithInPlace(result);
  return result.shallowReduce(reductionContext);
}

}  // namespace Poincare
