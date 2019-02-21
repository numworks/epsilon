#ifndef POINCARE_TANGENT_H
#define POINCARE_TANGENT_H

#include <poincare/approximation_helper.h>
#include <poincare/expression.h>

namespace Poincare {

class TangentNode final : public ExpressionNode {
public:

  // TreeNode
  size_t size() const override { return sizeof(TangentNode); }
  int numberOfChildren() const override;
#if POINCARE_TREE_LOG
  virtual void logNodeName(std::ostream & stream) const override {
    stream << "Tangent";
  }
#endif

  // Properties
  Type type() const override { return Type::Tangent; }
  float characteristicXRange(Context & context, Preferences::AngleUnit angleUnit) const override;

  // Complex
  bool isReal(Context & context) const override { return childAtIndex(0)->isReal(context); }

private:
  // Layout
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;

  // Simplication
  Expression shallowReduce(Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ReductionTarget target) override;

  // Evaluation
  template<typename T> static Complex<T> computeOnComplex(const std::complex<T> c, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit = Preferences::AngleUnit::Radian);
  Evaluation<float> approximate(SinglePrecision p, Context& context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const override {
    return ApproximationHelper::Map<float>(this, context, complexFormat, angleUnit,computeOnComplex<float>);
  }
  Evaluation<double> approximate(DoublePrecision p, Context& context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const override {
    return ApproximationHelper::Map<double>(this, context, complexFormat, angleUnit, computeOnComplex<double>);
  }
};

class Tangent final : public Expression {
public:
  Tangent(const TangentNode * n) : Expression(n) {}
  static Tangent Builder(Expression child);

  static constexpr Expression::FunctionHelper s_functionHelper = Expression::FunctionHelper("tan", 1, &UntypedBuilderOneChild<Tangent>);

  Expression shallowReduce(Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ExpressionNode::ReductionTarget target);
};

}

#endif
