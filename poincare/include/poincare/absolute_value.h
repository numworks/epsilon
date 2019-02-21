#ifndef POINCARE_ABSOLUTE_VALUE_H
#define POINCARE_ABSOLUTE_VALUE_H

#include <poincare/expression.h>
#include <poincare/rational.h>
#include <poincare/approximation_helper.h>

namespace Poincare {

class AbsoluteValueNode final : public ExpressionNode {
public:
  // TreeNode
  size_t size() const override { return sizeof(AbsoluteValueNode); }
  int numberOfChildren() const override;
#if POINCARE_TREE_LOG
  virtual void logNodeName(std::ostream & stream) const override {
    stream << "AbsoluteValue";
  }
#endif

  // Properties
  Type type() const override { return Type::AbsoluteValue; }
  Sign sign(Context * context) const override { return Sign::Positive; }
  Expression setSign(Sign s, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ReductionTarget target) override;

  // Complex
  bool isReal(Context & context) const override { return true; }

  // Approximation
  template<typename T> static Complex<T> computeOnComplex(const std::complex<T> c, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) {
    return Complex<T>::Builder(std::abs(c));
  }
  Evaluation<float> approximate(SinglePrecision p, Context& context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const override {
    return ApproximationHelper::Map<float>(this, context, complexFormat, angleUnit, computeOnComplex<float>);
  }
  Evaluation<double> approximate(DoublePrecision p, Context& context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const override {
    return ApproximationHelper::Map<double>(this, context, complexFormat, angleUnit, computeOnComplex<double>);
  }

  // Layout
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;

  // Simplification
  Expression shallowReduce(Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ReductionTarget target) override;
};

class AbsoluteValue final : public Expression {
friend class AbsoluteValueNode;
public:
  AbsoluteValue(const AbsoluteValueNode * n) : Expression(n) {}
  static AbsoluteValue Builder(Expression child);

  static constexpr Expression::FunctionHelper s_functionHelper = Expression::FunctionHelper("abs", 1, &UntypedBuilderOneChild<AbsoluteValue>);

  Expression shallowReduce(Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ExpressionNode::ReductionTarget target);
private:
  Expression setSign(ExpressionNode::Sign s, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit);
};

}

#endif
