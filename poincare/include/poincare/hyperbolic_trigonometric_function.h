#ifndef POINCARE_HYPERBOLIC_TRIGONOMETRIC_FUNCTION_H
#define POINCARE_HYPERBOLIC_TRIGONOMETRIC_FUNCTION_H

#include <poincare/expression.h>
#include <poincare/trigonometry.h>

namespace Poincare {

class HyperbolicTrigonometricFunctionNode : public ExpressionNode {
public:
  // TreeNode
  int numberOfChildren() const override { return 1; }
private:
  // Simplification
  Expression shallowReduce(ReductionContext reductionContext) override;
};

class HyperbolicTrigonometricFunction : public Expression {
public:
  HyperbolicTrigonometricFunction(const HyperbolicTrigonometricFunctionNode * n) : Expression(n) {}
  Expression shallowReduce(ExpressionNode::ReductionContext reductionContext);
};

}

#endif
