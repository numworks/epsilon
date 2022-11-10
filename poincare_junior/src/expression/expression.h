#ifndef POINCARE_EXPRESSIONS_EXPRESSION_H
#define POINCARE_EXPRESSIONS_EXPRESSION_H

#include <poincare_junior/src/memory/type_block.h>

namespace Poincare {

class EExpression {
public:
  static void BasicReduction(TypeBlock * block);
  static void ShallowBeautify(TypeBlock * block) {}
  static float Approximate(const TypeBlock * block);
  /* Approximation, defaultReduction, createLayout */
  // IsCommutative?
  // reduceMatrix?
  // diff?
  // Policy based Design:
  // EExpressionInterface<LayoutPolicy, SimplificationPolicy>...
  // What about decreasing v-table sizes?
  // EExpressionInterface
  // InternalEExpressionInterface
  // AlgebraicEExpressionInterface, FunctionEExpressionInterface etc?
protected:
  // TODO: tidy somewhere else
  static void ProjectionReduction(TypeBlock * block, TypeBlock * (*PushProjectedEExpression)(), TypeBlock * (*PushInverse)());
};

}

#endif


