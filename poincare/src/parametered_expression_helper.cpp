#include <poincare/parametered_expression_helper.h>
#include <poincare/symbol.h>
#include <string.h>
#include <assert.h>

namespace Poincare {

Expression ParameteredExpressionHelper::ReplaceUnknownInExpression(Expression e, const Symbol & symbolToReplace, const Symbol & unknownSymbol) {
  assert(!e.isUninitialized());
  assert(e.type() == ExpressionNode::Type::Integral
      || e.type() == ExpressionNode::Type::Derivative
      || e.type() == ExpressionNode::Type::Sum
      || e.type() == ExpressionNode::Type::Product);
  assert(!symbolToReplace.isUninitialized());
  assert(symbolToReplace.type() == ExpressionNode::Type::Symbol);

  int numberOfChildren = e.numberOfChildren();
  assert(numberOfChildren > 2);

  Expression child1 = e.childAtIndex(1);
  assert(child1.type() == ExpressionNode::Type::Symbol);

  Symbol& parameterChild = static_cast<Symbol &>(child1);
  if (strcmp(parameterChild.name(), symbolToReplace.name()) != 0) {
    return e.defaultReplaceUnknown(symbolToReplace, unknownSymbol);
  }
  for (int i = 2; i < numberOfChildren; i++) {
    e.childAtIndex(i).replaceUnknown(symbolToReplace, unknownSymbol);
  }
  return e;
}

}
