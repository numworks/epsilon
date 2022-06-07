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
  void logNodeName(std::ostream & stream) const override {
    stream << "ComplexArgument";
  }
#endif

  // Properties
  Type type() const override { return Type::ComplexArgument; }
private:
  // Layout
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  // Simplification
  Expression shallowReduce(ReductionContext reductionContext) override;
  LayoutShape leftLayoutShape() const override { return LayoutShape::MoreLetters; };
  LayoutShape rightLayoutShape() const override { return LayoutShape::BoundaryPunctuation; }

  // Evaluation
  template<typename T> static Complex<T> computeOnComplex(const std::complex<T> c, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit);
  Evaluation<float> approximate(SinglePrecision p, ApproximationContext approximationContext) const override {
    return ApproximationHelper::MapOneChild<float>(this, approximationContext, computeOnComplex<float>);
  }
  Evaluation<double> approximate(DoublePrecision p, ApproximationContext approximationContext) const override {
    return ApproximationHelper::MapOneChild<double>(this, approximationContext, computeOnComplex<double>);
  }
};

class ComplexArgument final : public Expression {
public:
  ComplexArgument(const ComplexArgumentNode * n) : Expression(n) {}
  static ComplexArgument Builder(Expression child) { return TreeHandle::FixedArityBuilder<ComplexArgument, ComplexArgumentNode>({child}); }

  static constexpr Expression::FunctionHelper s_functionHelper = Expression::FunctionHelper("arg", 1, Initializer<ComplexArgumentNode>, sizeof(ComplexArgumentNode));

  Expression shallowReduce(ExpressionNode::ReductionContext reductionContext);
};

}

#endif
