#ifndef POINCARE_ABSOLUTE_VALUE_H
#define POINCARE_ABSOLUTE_VALUE_H

#include <poincare/expression.h>
#include <poincare/approximation_helper.h>
#include <poincare/layout_helper.h>
#include <poincare/serialization_helper.h>

namespace Poincare {

class AbsoluteValueNode final : public ExpressionNode {
public:
  // TreeNode
  size_t size() const override { return sizeof(AbsoluteValueNode); }
  int numberOfChildren() const override { return 1; }
#if POINCARE_TREE_LOG
  virtual void logNodeName(std::ostream & stream) const override {
    stream << "AbsoluteValue";
  }
#endif

  // Properties
  Type type() const override { return Type::AbsoluteValue; }
  Sign sign() const override { return Sign::Positive; }
  Expression setSign(Sign s, Context & context, Preferences::AngleUnit angleUnit) override;

  // Approximation
  template<typename T> static Complex<T> computeOnComplex(const std::complex<T> c, Preferences::AngleUnit angleUnit) {
    return Complex<T>(std::abs(c));
  }
  Evaluation<float> approximate(SinglePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override {
    return ApproximationHelper::Map<float>(this, context, angleUnit, computeOnComplex<float>);
  }
  Evaluation<double> approximate(DoublePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override {
    return ApproximationHelper::Map<double>(this, context, angleUnit, computeOnComplex<double>);
  }

  // Layout
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override {
    return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, "abs");
  }

  // Simplification
  Expression shallowReduce(Context & context, Preferences::AngleUnit angleUnit) override;
};

class AbsoluteValue final : public Expression {
friend class AbsoluteValueNode;
public:
  AbsoluteValue();
  AbsoluteValue(const AbsoluteValueNode * n) : Expression(n) {}
  explicit AbsoluteValue(Expression operand) : AbsoluteValue() {
    replaceChildAtIndexInPlace(0, operand);
  }
  Expression shallowReduce(Context & context, Preferences::AngleUnit angleUnit);
private:
  Expression setSign(ExpressionNode::Sign s, Context & context, Preferences::AngleUnit angleUnit);
};

}

#endif
