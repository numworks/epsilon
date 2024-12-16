#include <poincare/old/addition.h>
#include <poincare/old/arc_tangent.h>
#include <poincare/old/complex_cartesian.h>
#include <poincare/old/constant.h>
#include <poincare/old/derivative.h>
#include <poincare/old/division.h>
#include <poincare/old/expression_node.h>
#include <poincare/old/old_expression.h>
#include <poincare/old/power.h>
#include <poincare/old/rational.h>
#include <poincare/old/sign_function.h>
#include <poincare/old/simplification_helper.h>
#include <poincare/old/square_root.h>
#include <poincare/old/subtraction.h>
#include <poincare/old/symbol.h>
#include <poincare/old/undefined.h>

namespace Poincare {

OExpression ExpressionNode::replaceSymbolWithExpression(
    const SymbolAbstract& symbol, const OExpression& expression) {
  return OExpression(this).deepReplaceSymbolWithExpression(symbol, expression);
}

int ExpressionNode::polynomialDegree(Context* context,
                                     const char* symbolName) const {
  assert(false);
  return 0;
}

int ExpressionNode::getPolynomialCoefficients(
    Context* context, const char* symbolName,
    OExpression coefficients[]) const {
  return OExpression(this).defaultGetPolynomialCoefficients(
      polynomialDegree(context, symbolName), context, symbolName, coefficients);
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
  ExpressionNode::Type type1 = e1->otype();
  if (ignoreParentheses && type1 == Type::Parenthesis) {
    return SimplificationOrder(e1->childAtIndex(0), e2, ascending,
                               ignoreParentheses);
  }
  ExpressionNode::Type type2 = e2->otype();
  if (ignoreParentheses && type2 == Type::Parenthesis) {
    return SimplificationOrder(e1, e2->childAtIndex(0), ascending,
                               ignoreParentheses);
  }
  assert((type1 == Type::Expression) == (type2 == Type::Expression));
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
    if (otype() == t) {
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
