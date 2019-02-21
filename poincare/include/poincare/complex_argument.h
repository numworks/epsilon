#ifndef POINCARE_COMPLEX_ARGUMENT_H
#define POINCARE_COMPLEX_ARGUMENT_H

#include <poincare/approximation_helper.h>
#include <poincare/expression.h>

namespace Poincare {

class ComplexArgumentNode final : public ExpressionNode  {
public:

  // TreeNode
  size_t size() const override { return sizeof(ComplexArgumentNode); }
  int numberOfChildren() const override;
#if POINCARE_TREE_LOG
  virtual void logNodeName(std::ostream & stream) const override {
    stream << "ComplexArgument";
  }
#endif
  // Complex
  bool isReal(Context & context) const override { return true; }

  // Properties
  Type type() const override { return Type::ComplexArgument; }
private:
  // Layout
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  // Simplification
  Expression shallowReduce(Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ReductionTarget target) override;
  // Evaluation
  template<typename T> static Complex<T> computeOnComplex(const std::complex<T> c, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit);
  Evaluation<float> approximate(SinglePrecision p, Context& context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const override {
    return ApproximationHelper::Map<float>(this, context, complexFormat, angleUnit,computeOnComplex<float>);
  }
  Evaluation<double> approximate(DoublePrecision p, Context& context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const override {
    return ApproximationHelper::Map<double>(this, context, complexFormat, angleUnit, computeOnComplex<double>);
  }
};

class ComplexArgument final : public Expression {
public:
  ComplexArgument(const ComplexArgumentNode * n) : Expression(n) {}
  static ComplexArgument Builder(Expression child);
  static Expression UntypedBuilder(Expression children) { return Builder(children.childAtIndex(0)); }
  static constexpr Expression::FunctionHelper s_functionHelper = Expression::FunctionHelper("arg", 1, &UntypedBuilder);

  Expression shallowReduce(Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ExpressionNode::ReductionTarget target);
};

}

#endif
