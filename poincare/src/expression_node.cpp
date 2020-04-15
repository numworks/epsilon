#include <poincare/expression_node.h>
#include <poincare/expression.h>
#include <poincare/addition.h>
#include <poincare/arc_tangent.h>
#include <poincare/complex_cartesian.h>
#include <poincare/division.h>
#include <poincare/power.h>
#include <poincare/rational.h>
#include <poincare/sign_function.h>
#include <poincare/square_root.h>
#include <poincare/subtraction.h>
#include <poincare/constant.h>
#include <poincare/undefined.h>

namespace Poincare {

Expression ExpressionNode::replaceSymbolWithExpression(const SymbolAbstract & symbol, const Expression & expression) {
  return Expression(this).defaultReplaceSymbolWithExpression(symbol, expression);
}

Expression ExpressionNode::setSign(Sign s, ReductionContext reductionContext) {
  assert(false);
  return Expression();
}

int ExpressionNode::polynomialDegree(Context * context, const char * symbolName) const {
  for (ExpressionNode * c : children()) {
    if (c->polynomialDegree(context, symbolName) != 0) {
      return -1;
    }
  }
  return 0;
}

int ExpressionNode::getPolynomialCoefficients(Context * context, const char * symbolName, Expression coefficients[], ExpressionNode::SymbolicComputation symbolicComputation) const {
  return Expression(this).defaultGetPolynomialCoefficients(context, symbolName, coefficients);
}

Expression ExpressionNode::deepReplaceReplaceableSymbols(Context * context, bool * didReplace, bool replaceFunctionsOnly, int parameteredAncestorsCount) {
  return Expression(this).defaultReplaceReplaceableSymbols(context, didReplace, replaceFunctionsOnly, parameteredAncestorsCount);
}

int ExpressionNode::getVariables(Context * context, isVariableTest isVariable, char * variables, int maxSizeVariable, int nextVariableIndex) const {
  for (ExpressionNode * c : children()) {
    int n = c->getVariables(context, isVariable, variables, maxSizeVariable, nextVariableIndex);
    if (n < 0) {
      return n;
    }
    nextVariableIndex = n;
  }
  return nextVariableIndex;
}

float ExpressionNode::characteristicXRange(Context * context, Preferences::AngleUnit angleUnit) const {
  /* A expression has a characteristic range if at least one of its childAtIndex has
   * one and the other are x-independant. We keep the biggest interesting range
   * among the childAtIndex interesting ranges. */
  float range = 0.0f;
  for (ExpressionNode * c : children()) {
    float opRange = c->characteristicXRange(context, angleUnit);
    if (std::isnan(opRange)) {
      return NAN;
    } else if (range < opRange) {
      range = opRange;
    }
  }
  return range;
}

int ExpressionNode::SimplificationOrder(const ExpressionNode * e1, const ExpressionNode * e2, bool ascending, bool canBeInterrupted, bool ignoreParentheses) {
  // Depending on ignoreParentheses, check if e1 or e2 are parenthesis
  ExpressionNode::Type type1 = e1->type();
  if (ignoreParentheses && type1 == Type::Parenthesis) {
    if (canBeInterrupted && Expression::ShouldStopProcessing()) {
      return -1;
    }
    return SimplificationOrder(e1->childAtIndex(0), e2, ascending, canBeInterrupted, ignoreParentheses);
  }
  ExpressionNode::Type type2 = e2->type();
  if (ignoreParentheses && type2 == Type::Parenthesis) {
    return SimplificationOrder(e1, e2->childAtIndex(0), ascending, canBeInterrupted, ignoreParentheses);
  }
  if (type1 > type2) {
    if (canBeInterrupted && Expression::ShouldStopProcessing()) {
      return 1;
    }
    return -(e2->simplificationOrderGreaterType(e1, ascending, canBeInterrupted, ignoreParentheses));
  } else if (type1 == type2) {
    return e1->simplificationOrderSameType(e2, ascending, canBeInterrupted, ignoreParentheses);
  } else {
    if (canBeInterrupted && Expression::ShouldStopProcessing()) {
      return -1;
    }
    return e1->simplificationOrderGreaterType(e2, ascending, canBeInterrupted, ignoreParentheses);
  }
}

int ExpressionNode::simplificationOrderSameType(const ExpressionNode * e, bool ascending, bool canBeInterrupted, bool ignoreParentheses) const {
  int index = 0;
  for (ExpressionNode * c : children()) {
    // The NULL node is the least node type.
    if (e->numberOfChildren() <= index) {
      return 1;
    }
    int childIOrder = SimplificationOrder(c, e->childAtIndex(index), ascending, canBeInterrupted, ignoreParentheses);
    if (childIOrder != 0) {
      return childIOrder;
    }
    index++;
  }
  // The NULL node is the least node type.
  if (e->numberOfChildren() > numberOfChildren()) {
    return ascending ? -1 : 1;
  }
  return 0;
}

void ExpressionNode::deepReduceChildren(ExpressionNode::ReductionContext reductionContext) {
  Expression(this).defaultDeepReduceChildren(reductionContext);
}

Expression ExpressionNode::shallowReduce(ReductionContext reductionContext) {
  return Expression(this).defaultShallowReduce();
}

Expression ExpressionNode::shallowBeautify(ReductionContext reductionContext) {
  return Expression(this).defaultShallowBeautify();
}

bool ExpressionNode::isOfType(Type * types, int length) const {
  for (int i = 0; i < length; i++) {
    if (type() == types[i]) {
      return true;
    }
  }
  return false;
}

Expression ExpressionNode::getUnit() const {
  return Undefined::Builder();
}

void ExpressionNode::setChildrenInPlace(Expression other) {
  Expression(this).defaultSetChildrenInPlace(other);
}

Expression ExpressionNode::denominator(ReductionContext reductionContext) const {
  return Expression();
}

}
