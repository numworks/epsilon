#ifndef POINCARE_HYPERBOLIC_TRIGONOMETRIC_FUNCTION_H
#define POINCARE_HYPERBOLIC_TRIGONOMETRIC_FUNCTION_H

#include "old_expression.h"
#include "rational.h"
#include "trigonometry.h"

namespace Poincare {

class HyperbolicTrigonometricFunctionNode : public ExpressionNode {
  friend class HyperbolicTrigonometricFunction;

 public:
  // PoolObject
  int numberOfChildren() const override { return 1; }

 private:
  // Simplification
  LayoutShape leftLayoutShape() const override {
    return LayoutShape::MoreLetters;
  };
  LayoutShape rightLayoutShape() const override {
    return LayoutShape::BoundaryPunctuation;
  }
  OExpression shallowReduce(const ReductionContext& reductionContext) override;
  virtual bool isNotableValue(OExpression e, Context* context) const {
    return e.isNull(context) == OMG::Troolean::True;
  }
  virtual OExpression imageOfNotableValue() const {
    return Rational::Builder(0);
  }
};

class HyperbolicTrigonometricFunction : public OExpression {
 public:
  HyperbolicTrigonometricFunction(const HyperbolicTrigonometricFunctionNode* n)
      : OExpression(n) {}
  OExpression shallowReduce(ReductionContext reductionContext);

 private:
  HyperbolicTrigonometricFunctionNode* node() const {
    return static_cast<HyperbolicTrigonometricFunctionNode*>(
        OExpression::node());
  }
};

}  // namespace Poincare

#endif
