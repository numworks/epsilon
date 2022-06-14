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
#include <poincare/symbol.h>
#include <poincare/simplification_helper.h>

namespace Poincare {

Expression ExpressionNode::replaceSymbolWithExpression(const SymbolAbstract & symbol, const Expression & expression) {
  return Expression(this).defaultReplaceSymbolWithExpression(symbol, expression);
}

int ExpressionNode::polynomialDegree(Context * context, const char * symbolName) const {
  for (ExpressionNode * c : children()) {
    if (c->polynomialDegree(context, symbolName) != 0) {
      return -1;
    }
  }
  return 0;
}

int ExpressionNode::getPolynomialCoefficients(Context * context, const char * symbolName, Expression coefficients[]) const {
  return Expression(this).defaultGetPolynomialCoefficients(context, symbolName, coefficients);
}

Expression ExpressionNode::deepReplaceReplaceableSymbols(Context * context, bool * isCircular, int maxSymbolsToReplace, int parameteredAncestorsCount, SymbolicComputation symbolicComputation) {
  return Expression(this).defaultReplaceReplaceableSymbols(context, isCircular, maxSymbolsToReplace, parameteredAncestorsCount, symbolicComputation);
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

int ExpressionNode::SimplificationOrder(const ExpressionNode * e1, const ExpressionNode * e2, bool ascending, bool ignoreParentheses) {
  // Depending on ignoreParentheses, check if e1 or e2 are parenthesis
  ExpressionNode::Type type1 = e1->type();
  if (ignoreParentheses && type1 == Type::Parenthesis) {
    return SimplificationOrder(e1->childAtIndex(0), e2, ascending, ignoreParentheses);
  }
  ExpressionNode::Type type2 = e2->type();
  if (ignoreParentheses && type2 == Type::Parenthesis) {
    return SimplificationOrder(e1, e2->childAtIndex(0), ascending, ignoreParentheses);
  }
  if (type1 > type2) {
    return -(e2->simplificationOrderGreaterType(e1, ascending, ignoreParentheses));
  } else if (type1 == type2) {
    return e1->simplificationOrderSameType(e2, ascending, ignoreParentheses);
  } else {
    return e1->simplificationOrderGreaterType(e2, ascending, ignoreParentheses);
  }
}

int ExpressionNode::simplificationOrderSameType(const ExpressionNode * e, bool ascending, bool ignoreParentheses) const {
  int index = 0;
  for (ExpressionNode * c : children()) {
    // The NULL node is the least node type.
    if (e->numberOfChildren() <= index) {
      return 1;
    }
    int childIOrder = SimplificationOrder(c, e->childAtIndex(index), ascending, ignoreParentheses);
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

Expression ExpressionNode::shallowReduce(const ReductionContext& reductionContext) {
  Expression e(this);
  Expression res = SimplificationHelper::shallowReduceUndefined(e);
  if (!res.isUninitialized()) {
    return res;
  }
  return e;
}

Expression ExpressionNode::shallowBeautify(const ReductionContext& reductionContext) {
  return Expression(this).defaultShallowBeautify();
}

bool ExpressionNode::derivate(const ReductionContext& reductionContext, Symbol symbol, Expression symbolValue) {
  return Expression(this).defaultDidDerivate();
}

Expression ExpressionNode::unaryFunctionDifferential(const ReductionContext& reductionContext) {
  return Expression(this).defaultUnaryFunctionDifferential();
}

bool ExpressionNode::isOfType(std::initializer_list<ExpressionNode::Type> types) const {
  for (ExpressionNode::Type t : types) {
    if (type() == t) {
      return true;
    }
  }
  return false;
}

bool ExpressionNode::hasMatrixOrListChild(Context * context) const {
  for (ExpressionNode * c : children()) {
    if (Expression(c).deepIsMatrix(context) || Expression(c).deepIsList(context)) {
      return true;
    }
  }
  return false;
}

Expression ExpressionNode::removeUnit(Expression * unit) {
  return Expression(this);
}

void ExpressionNode::setChildrenInPlace(Expression other) {
  Expression(this).defaultSetChildrenInPlace(other);
}

}
