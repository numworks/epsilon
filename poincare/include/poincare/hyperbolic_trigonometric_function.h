#ifndef POINCARE_HYPERBOLIC_TRIGONOMETRIC_FUNCTION_H
#define POINCARE_HYPERBOLIC_TRIGONOMETRIC_FUNCTION_H

#include <poincare/expression.h>
#include <poincare/serialization_helper.h>
#include <poincare/trigonometry.h>

namespace Poincare {

class HyperbolicTrigonometricFunctionNode : public ExpressionNode {
public:
  // TreeNode
  int numberOfChildren() const override { return 1; }
private:
  // Layout
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override {
    return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, name());
  }
  virtual const char * name() const = 0;
  // Simplification
  Expression shallowReduce(Context & context, Preferences::AngleUnit angleUnit) override;
};

class HyperbolicTrigonometricFunction : public Expression {
public:
  HyperbolicTrigonometricFunction(const HyperbolicTrigonometricFunctionNode * n) : Expression(n) {}
  Expression shallowReduce(Context & context, Preferences::AngleUnit angleUnit);
};

}

#endif
