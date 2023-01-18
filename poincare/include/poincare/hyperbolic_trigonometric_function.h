#ifndef POINCARE_HYPERBOLIC_TRIGONOMETRIC_FUNCTION_H
#define POINCARE_HYPERBOLIC_TRIGONOMETRIC_FUNCTION_H

#include <poincare/expression.h>
#include <poincare/trigonometry.h>
#include <poincare/rational.h>

namespace Poincare {

class HyperbolicTrigonometricFunctionNode : public ExpressionNode {
  friend class HyperbolicTrigonometricFunction;
public:
  // TreeNode
  int numberOfChildren() const override { return 1; }
private:
  // Simplification
  LayoutShape leftLayoutShape() const override { return LayoutShape::MoreLetters; };
  LayoutShape rightLayoutShape() const override { return LayoutShape::BoundaryPunctuation; }
  Expression shallowReduce(const ReductionContext& reductionContext) override;
  virtual bool isNotableValue(Expression e, Context * context) const { return e.isNull(context) == TrinaryBoolean::True; }
  virtual Expression imageOfNotableValue() const { return Rational::Builder(0); }
};

class HyperbolicTrigonometricFunction : public Expression {
public:
  HyperbolicTrigonometricFunction(const HyperbolicTrigonometricFunctionNode * n) : Expression(n) {}
  Expression shallowReduce(ReductionContext reductionContext);
private:
  HyperbolicTrigonometricFunctionNode * node() const { return static_cast<HyperbolicTrigonometricFunctionNode *>(Expression::node()); }
};

}

#endif
