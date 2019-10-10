#include <poincare/opposite.h>
#include <poincare/addition.h>
#include <poincare/code_point_layout.h>
#include <poincare/constant.h>
#include <poincare/horizontal_layout.h>
#include <poincare/layout_helper.h>
#include <poincare/multiplication.h>
#include <poincare/rational.h>
#include <poincare/serialization_helper.h>
#include <assert.h>
#include <cmath>
#include <stdlib.h>
#include <utility>

namespace Poincare {

int OppositeNode::polynomialDegree(Context * context, const char * symbolName) const {
  return childAtIndex(0)->polynomialDegree(context, symbolName);
}

ExpressionNode::Sign OppositeNode::sign(Context * context) const {
  Sign child0Sign = childAtIndex(0)->sign(context);
  if (child0Sign == Sign::Positive) {
    return Sign::Negative;
  }
  if (child0Sign == Sign::Negative) {
    return Sign::Positive;
  }
  return ExpressionNode::sign(context);
}

/* Layout */

bool OppositeNode::childAtIndexNeedsUserParentheses(const Expression & child, int childIndex) const {
  assert(childIndex == 0);
  if (child.isNumber() && static_cast<const Number &>(child).sign() == Sign::Negative) {
    return true;
  }
  if (child.type() == Type::Conjugate) {
    return childAtIndexNeedsUserParentheses(child.childAtIndex(0), 0);
  }
  Type types[] = {Type::Addition, Type::Subtraction, Type::Opposite};
  return child.isOfType(types, 3);
}

Layout OppositeNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  HorizontalLayout result = HorizontalLayout::Builder(CodePointLayout::Builder('-'));
  if (childAtIndex(0)->type() == Type::Opposite) {
    result.addOrMergeChildAtIndex(LayoutHelper::Parentheses(childAtIndex(0)->createLayout(floatDisplayMode, numberOfSignificantDigits), false), 1, false);
  } else {
    result.addOrMergeChildAtIndex(childAtIndex(0)->createLayout(floatDisplayMode, numberOfSignificantDigits), 1, false);
  }
  return std::move(result);
}

int OppositeNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  if (bufferSize == 0) {
    return -1;
  }
  buffer[bufferSize-1] = 0;
  if (bufferSize == 1) { return 0; }
  int numberOfChar = SerializationHelper::CodePoint(buffer, bufferSize, '-');
  if (numberOfChar >= bufferSize - 1) {
    return bufferSize - 1;
  }
  numberOfChar += childAtIndex(0)->serialize(buffer+numberOfChar, bufferSize-numberOfChar, floatDisplayMode, numberOfSignificantDigits);
  return numberOfChar;
}

Expression OppositeNode::shallowReduce(ReductionContext reductionContext) {
  return Opposite(this).shallowReduce(reductionContext);
}

/* Simplification */

Expression Opposite::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
  Expression result = Expression::defaultShallowReduce();
  if (result.isUndefined()) {
    return result;
  }
  Expression child = result.childAtIndex(0);
  result = Multiplication::Builder(Rational::Builder(-1), child);
  replaceWithInPlace(result);
  return result.shallowReduce(reductionContext);
}

}
