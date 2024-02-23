#include <poincare/addition.h>
#include <poincare/arc_tangent.h>
#include <poincare/complex_cartesian.h>
#include <poincare/constant.h>
#include <poincare/derivative.h>
#include <poincare/division.h>
#include <poincare/expression.h>
#include <poincare/expression_node.h>
#include <poincare/power.h>
#include <poincare/rational.h>
#include <poincare/sign_function.h>
#include <poincare/simplification_helper.h>
#include <poincare/square_root.h>
#include <poincare/subtraction.h>
#include <poincare/symbol.h>
#include <poincare/undefined.h>

namespace Poincare {

OExpression ExpressionNode::replaceSymbolWithExpression(
    const SymbolAbstract& symbol, const OExpression& expression) {
  return OExpression(this).deepReplaceSymbolWithExpression(symbol, expression);
}

int ExpressionNode::polynomialDegree(Context* context,
                                     const char* symbolName) const {
  for (ExpressionNode* c : children()) {
    if (c->polynomialDegree(context, symbolName) != 0) {
      return -1;
    }
  }
  return 0;
}

int ExpressionNode::getPolynomialCoefficients(
    Context* context, const char* symbolName,
    OExpression coefficients[]) const {
  return OExpression(this).defaultGetPolynomialCoefficients(
      polynomialDegree(context, symbolName), context, symbolName, coefficients);
}

bool ExpressionNode::involvesCircularity(Context* context, int maxDepth,
                                         const char** visitedSymbols,
                                         int numberOfVisitedSymbols) {
  int nChildren = numberOfChildren();
  for (int i = 0; i < nChildren; i++) {
    if (childAtIndex(i)->involvesCircularity(context, maxDepth, visitedSymbols,
                                             numberOfVisitedSymbols)) {
      return true;
    }
  }
  return false;
}

OExpression ExpressionNode::deepReplaceReplaceableSymbols(
    Context* context, TrinaryBoolean* isCircular, int parameteredAncestorsCount,
    SymbolicComputation symbolicComputation) {
  return OExpression(this).defaultReplaceReplaceableSymbols(
      context, isCircular, parameteredAncestorsCount, symbolicComputation);
}

int ExpressionNode::getVariables(Context* context, isVariableTest isVariable,
                                 char* variables, int maxSizeVariable,
                                 int nextVariableIndex) const {
  for (ExpressionNode* c : children()) {
    int n = c->getVariables(context, isVariable, variables, maxSizeVariable,
                            nextVariableIndex);
    if (n < 0) {
      return n;
    }
    nextVariableIndex = n;
  }
  return nextVariableIndex;
}

int ExpressionNode::SimplificationOrder(const ExpressionNode* e1,
                                        const ExpressionNode* e2,
                                        bool ascending,
                                        bool ignoreParentheses) {
  // Depending on ignoreParentheses, check if e1 or e2 are parenthesis
  ExpressionNode::Type type1 = e1->type();
  if (ignoreParentheses && type1 == Type::Parenthesis) {
    return SimplificationOrder(e1->childAtIndex(0), e2, ascending,
                               ignoreParentheses);
  }
  ExpressionNode::Type type2 = e2->type();
  if (ignoreParentheses && type2 == Type::Parenthesis) {
    return SimplificationOrder(e1, e2->childAtIndex(0), ascending,
                               ignoreParentheses);
  }
  if (type1 > type2) {
    return -(
        e2->simplificationOrderGreaterType(e1, ascending, ignoreParentheses));
  } else if (type1 == type2) {
    return e1->simplificationOrderSameType(e2, ascending, ignoreParentheses);
  } else {
    return e1->simplificationOrderGreaterType(e2, ascending, ignoreParentheses);
  }
}

int ExpressionNode::simplificationOrderSameType(const ExpressionNode* e,
                                                bool ascending,
                                                bool ignoreParentheses) const {
  int index = 0;
  for (ExpressionNode* c : children()) {
    // The NULL node is the least node type.
    if (e->numberOfChildren() <= index) {
      return 1;
    }
    int childIOrder = SimplificationOrder(c, e->childAtIndex(index), ascending,
                                          ignoreParentheses);
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

OExpression ExpressionNode::shallowReduce(
    const ReductionContext& reductionContext) {
  OExpression e(this);
  ReductionContext alterableContext = reductionContext;
  OExpression res =
      SimplificationHelper::defaultShallowReduce(e, &alterableContext);
  if (!res.isUninitialized()) {
    return res;
  }
  return e;
}

OExpression ExpressionNode::shallowBeautify(
    const ReductionContext& reductionContext) {
  return OExpression(this).defaultShallowBeautify();
}

bool ExpressionNode::derivate(const ReductionContext& reductionContext,
                              Symbol symbol, OExpression symbolValue) {
  OExpression e =
      Derivative::DefaultDerivate(OExpression(this), reductionContext, symbol);
  return !e.isUninitialized();
}

OExpression ExpressionNode::unaryFunctionDifferential(
    const ReductionContext& reductionContext) {
  return OExpression(this).defaultUnaryFunctionDifferential();
}

bool ExpressionNode::isOfType(
    std::initializer_list<ExpressionNode::Type> types) const {
  for (ExpressionNode::Type t : types) {
    if (type() == t) {
      return true;
    }
  }
  return false;
}

bool ExpressionNode::hasMatrixOrListChild(Context* context,
                                          bool isReduced) const {
  for (ExpressionNode* c : children()) {
    if (OExpression(c).deepIsMatrix(context, true, isReduced) ||
        OExpression(c).deepIsList(context)) {
      return true;
    }
  }
  return false;
}

OExpression ExpressionNode::removeUnit(OExpression* unit) {
  return OExpression(this);
}

void ExpressionNode::setChildrenInPlace(OExpression other) {
  OExpression(this).defaultSetChildrenInPlace(other);
}

}  // namespace Poincare
