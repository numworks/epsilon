#ifndef POINCARE_EXPRESSIONS_EXPRESSION_H
#define POINCARE_EXPRESSIONS_EXPRESSION_H

#include "../type_block.h"

namespace Poincare {

class Expression {
public:
  static void BasicReduction(TypeBlock * block);
  static void ShallowBeautify(TypeBlock * block) {}
  static float Approximate(const TypeBlock * block);
  /* Approximation, defaultReduction, createLayout */
  // IsCommutative?
  // reduceMatrix?
  // diff?
  // Policy based Design:
  // ExpressionInterface<LayoutPolicy, SimplificationPolicy>...
  // What about decreasing v-table sizes?
  // ExpressionInterface
  // InternalExpressionInterface
  // AlgebraicExpressionInterface, FunctionExpressionInterface etc?
protected:
  // TODO: tidy somewhere else
  static void ProjectionReduction(TypeBlock * block, TypeBlock * (*PushProjectedExpression)(), TypeBlock * (*PushInverse)());
};

}

#endif


