#include <poincare/opposite.h>
#include <poincare/addition.h>
#include <poincare/code_point_layout.h>
#include <poincare/constant.h>
#include <poincare/horizontal_layout.h>
#include <poincare/layout_helper.h>
#include <poincare/multiplication.h>
#include <poincare/rational.h>
#include <poincare/serialization_helper.h>

extern "C" {
#include <cmath>
#include <assert.h>
#include <stdlib.h>
}

namespace Poincare {

int OppositeNode::polynomialDegree(Context & context, const char * symbolName) const {
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

bool OppositeNode::childNeedsParenthesis(const TreeNode * child) const {
  if (static_cast<const ExpressionNode *>(child)->isNumber() && Number(static_cast<const NumberNode *>(child)).sign() == Sign::Negative) {
    return true;
  }
  Type types[] = {Type::Addition, Type::Subtraction, Type::Opposite};
  return static_cast<const ExpressionNode *>(child)->isOfType(types, 3);
}

Layout OppositeNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  HorizontalLayout result = HorizontalLayout::Builder(CodePointLayout::Builder('-'));
  if (childAtIndex(0)->type() == Type::Opposite) {
    result.addOrMergeChildAtIndex(LayoutHelper::Parentheses(childAtIndex(0)->createLayout(floatDisplayMode, numberOfSignificantDigits), false), 1, false);
  } else {
    result.addOrMergeChildAtIndex(childAtIndex(0)->createLayout(floatDisplayMode, numberOfSignificantDigits), 1, false);
  }
  return result;
}

int OppositeNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  if (bufferSize == 0) {
    return -1;
  }
  buffer[bufferSize-1] = 0;
  int numberOfChar = 0;
  if (bufferSize == 1) { return 0; }
  numberOfChar += SerializationHelper::CodePoint(buffer + numberOfChar, bufferSize - numberOfChar, '-');
  if (numberOfChar >= bufferSize - 1) {
    return bufferSize - 1;
  }
  numberOfChar += childAtIndex(0)->serialize(buffer+numberOfChar, bufferSize-numberOfChar, floatDisplayMode, numberOfSignificantDigits);
  buffer[numberOfChar] = 0;
  return numberOfChar;
}

Expression OppositeNode::shallowReduce(Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ReductionTarget target, bool symbolicComputation) {
  return Opposite(this).shallowReduce(context, complexFormat, angleUnit, target);
}

/* Simplification */

Expression Opposite::shallowReduce(Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ExpressionNode::ReductionTarget target) {
  Expression result = Expression::defaultShallowReduce();
  if (result.isUndefined()) {
    return result;
  }
  Expression child = result.childAtIndex(0);
  result = Multiplication::Builder(Rational::Builder(-1), child);
  replaceWithInPlace(result);
  return result.shallowReduce(context, complexFormat, angleUnit, target);
}

}
