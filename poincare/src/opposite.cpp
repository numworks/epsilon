#include <poincare/opposite.h>
#include <poincare/char_layout_node.h>
#include <poincare/horizontal_layout_node.h>
#include <cmath>
#include <poincare/layout_engine.h>
//#include <poincare/multiplication.h>
#include <poincare/rational.h>
#include <poincare/simplification_engine.h>
extern "C" {
#include <assert.h>
#include <stdlib.h>
}

namespace Poincare {

OppositeNode * OppositeNode::FailedAllocationStaticNode() {
  static AllocationFailureExpressionNode<OppositeNode> failure;
  return &failure;
}

int OppositeNode::polynomialDegree(char symbolName) const {
  return childAtIndex(0)->polynomialDegree(symbolName);
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

bool OppositeNode::needsParenthesisWithParent(SerializableNode * e) const {
  Type types[] = {Type::Addition, Type::Subtraction, Type::Opposite, Type::Multiplication, Type::Division, Type::Power, Type::Factorial};
  return static_cast<ExpressionNode *>(e)->isOfType(types, 7);
}

LayoutRef OppositeNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  HorizontalLayoutRef result = HorizontalLayoutRef(CharLayoutRef('-'));
  if (childAtIndex(0)->type() == Type::Opposite) {
    result.addOrMergeChildAtIndex(LayoutEngine::createParenthesedLayout(childAtIndex(0)->createLayout(floatDisplayMode, numberOfSignificantDigits), false), 1, false);
  } else {
    result.addOrMergeChildAtIndex(childAtIndex(0)->createLayout(floatDisplayMode, numberOfSignificantDigits), 1, false);
  }
  return result;
}

int OppositeNode::writeTextInBuffer(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  if (bufferSize == 0) {
    return -1;
  }
  buffer[bufferSize-1] = 0;
  int numberOfChar = 0;
  if (bufferSize == 1) { return 0; }
  buffer[numberOfChar++] = '-';
  numberOfChar += childAtIndex(0)->writeTextInBuffer(buffer+numberOfChar, bufferSize-numberOfChar, floatDisplayMode, numberOfSignificantDigits);
  buffer[numberOfChar] = 0;
  return numberOfChar;
}

Expression OppositeNode::shallowReduce(Context& context, Preferences::AngleUnit angleUnit) const {
  return Opposite(this).shallowReduce(context, angleUnit);
}

/* Simplification */

Expression Opposite::shallowReduce(Context & context, Preferences::AngleUnit angleUnit) {
  Expression result = Expression::shallowReduce(context, angleUnit);
#if 0
  // TODO: handle isFailed or isUndefined
  if (result.hasFailure()) {
    return result;
  }
#endif
  Expression child = result.childAtIndex(0);
#if MATRIX_EXACT_REDUCING
#endif
  result = Multiplication(Rational(-1), child);
  return result.shallowReduce(context.angleUnit);
}
#if 0

ExpressionReference OppositeNode::shallowReduce(Context& context, Preferences::AngleUnit angleUnit) {
  ExpressionReference e = ExpressionNode::shallowReduce(context, angleUnit);
  if (e.node() != this) {
    return e;
  }
  const ExpressionReference child = ExpressionReference(childAtIndex(0));
#if MATRIX_EXACT_REDUCING
  if (op->type() == Type::Matrix) {
    return SimplificationEngine::map(this, context, angleUnit);
  }
#endif
  MultiplicationReference m = MultiplicationReference(RationalReference(-1), child);
  return m->node()->shallowReduce(context, angleUnit);
}
#endif

}
