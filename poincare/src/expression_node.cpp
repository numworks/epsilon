#include <poincare/expression_node.h>
#include <poincare/undefined.h>
#include <poincare/expression.h>

namespace Poincare {

Expression ExpressionNode::replaceSymbolWithExpression(char symbol, Expression expression) const {
  return Expression(this).privateReplaceSymbolWithExpression(symbol, expression);
}

Expression ExpressionNode::setSign(Sign s, Context & context, Preferences::AngleUnit angleUnit) const {
  assert(false);
  return Expression(nullptr);
}

int ExpressionNode::polynomialDegree(char symbolName) const {
  for (int i = 0; i < numberOfChildren(); i++) {
    if (childAtIndex(i)->polynomialDegree(symbolName) != 0) {
      return -1;
    }
  }
  return 0;
}

int ExpressionNode::getPolynomialCoefficients(char symbolName, Expression coefficients[]) const {
  int deg = polynomialDegree(symbolName);
  if (deg == 0) {
    coefficients[0] = Expression(this);
    return 0;
  }
  return -1;
}

int ExpressionNode::getVariables(isVariableTest isVariable, char * variables) const {
 int numberOfVariables = 0;
 for (int i = 0; i < numberOfChildren(); i++) {
   int n = childAtIndex(i)->getVariables(isVariable, variables);
   if (n < 0) {
     return -1;
   }
   numberOfVariables = n > numberOfVariables ? n : numberOfVariables;
 }
 return numberOfVariables;
}

float ExpressionNode::characteristicXRange(Context & context, Preferences::AngleUnit angleUnit) const {
  /* A expression has a characteristic range if at least one of its childAtIndex has
   * one and the other are x-independant. We keep the biggest interesting range
   * among the childAtIndex interesting ranges. */
  float range = 0.0f;
  for (int i = 0; i < numberOfChildren(); i++) {
    float opRange = childAtIndex(i)->characteristicXRange(context, angleUnit);
    if (std::isnan(opRange)) {
      return NAN;
    } else if (range < opRange) {
      range = opRange;
    }
  }
  return range;
}

int ExpressionNode::SimplificationOrder(const ExpressionNode * e1, const ExpressionNode * e2, bool canBeInterrupted) {
  if (e1->type() > e2->type()) {
    if (canBeInterrupted && Expression::shouldStopProcessing()) {
      return 1;
    }
    return -(e2->simplificationOrderGreaterType(e1, canBeInterrupted));
  } else if (e1->type() == e2->type()) {
    return e1->simplificationOrderSameType(e2, canBeInterrupted);
  } else {
    if (canBeInterrupted && Expression::shouldStopProcessing()) {
      return -1;
    }
    return e1->simplificationOrderGreaterType(e2, canBeInterrupted);
  }
}

Expression ExpressionNode::shallowReduce(Context & context, Preferences::AngleUnit angleUnit) const {
  return Expression(this).simpleShallowReduce(context, angleUnit);
}

Expression ExpressionNode::shallowBeautify(Context & context, Preferences::AngleUnit angleUnit) const {
  Expression result = Expression(this);
  return result;
}

bool ExpressionNode::isOfType(Type * types, int length) const {
  for (int i = 0; i < length; i++) {
    if (type() == types[i]) {
      return true;
    }
  }
  return false;
}

Expression ExpressionNode::denominator(Context & context, Preferences::AngleUnit angleUnit) const {
  return Expression(nullptr);
}

}
