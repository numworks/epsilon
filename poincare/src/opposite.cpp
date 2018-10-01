#include <poincare/opposite.h>
#include <poincare/char_layout.h>
#include <poincare/horizontal_layout.h>
#include <cmath>
#include <poincare/layout_helper.h>
#include <poincare/multiplication.h>
#include <poincare/rational.h>
#include <poincare/simplification_helper.h>
extern "C" {
#include <assert.h>
#include <stdlib.h>
}

namespace Poincare {

int OppositeNode::polynomialDegree(Context & context, const char * symbolName) const {
  return childAtIndex(0)->polynomialDegree(context, symbolName);
}

ExpressionNode::Sign OppositeNode::sign() const {
  if (childAtIndex(0)->sign() == Sign::Positive) {
    return Sign::Negative;
  }
  if (childAtIndex(0)->sign() == Sign::Negative) {
    return Sign::Positive;
  }
  return Sign::Unknown;
}

/* Layout */

bool OppositeNode::childNeedsParenthesis(const TreeNode * child) const {
  if (static_cast<const ExpressionNode *>(child)->isNumber() && static_cast<const ExpressionNode *>(child)->sign() == Sign::Negative) {
    return true;
  }
  Type types[] = {Type::Addition, Type::Subtraction, Type::Opposite};
  return static_cast<const ExpressionNode *>(child)->isOfType(types, 3);
}

Layout OppositeNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  HorizontalLayout result = HorizontalLayout(CharLayout('-'));
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
  buffer[numberOfChar++] = '-';
  numberOfChar += childAtIndex(0)->serialize(buffer+numberOfChar, bufferSize-numberOfChar, floatDisplayMode, numberOfSignificantDigits);
  buffer[numberOfChar] = 0;
  return numberOfChar;
}

Expression OppositeNode::shallowReduce(Context & context, Preferences::AngleUnit angleUnit, bool replaceSymbols) {
  return Opposite(this).shallowReduce(context, angleUnit, replaceSymbols);
}

/* Simplification */

Opposite::Opposite() : Expression(TreePool::sharedPool()->createTreeNode<OppositeNode>()) {}

Expression Opposite::shallowReduce(Context & context, Preferences::AngleUnit angleUnit, bool replaceSymbols) {
  Expression result = Expression::defaultShallowReduce(context, angleUnit);
  if (result.isUndefined()) {
    return result;
  }
  Expression child = result.childAtIndex(0);
#if MATRIX_EXACT_REDUCING
#endif
  result = Multiplication(Rational(-1), child);
  replaceWithInPlace(result);
  return result.shallowReduce(context, angleUnit);
}

}
