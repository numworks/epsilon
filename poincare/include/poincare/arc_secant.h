#ifndef POINCARE_ARC_SECANT_H
#define POINCARE_ARC_SECANT_H

#include <poincare/approximation_helper.h>
#include <poincare/expression.h>

namespace Poincare {

class ArcSecantNode final : public ExpressionNode {
public:
  // TreeNode
  size_t size() const override { return sizeof(ArcSecantNode); }
  int numberOfChildren() const override;
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "ArcSecant";
  }
#endif

  // Properties
  Type type() const override { return Type::ArcSecant; }

  template<typename T> static Complex<T> computeOnComplex(const std::complex<T> c, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit = Preferences::AngleUnit::Radian);

private:
  // Layout
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  // Simplication
  Expression shallowReduce(ReductionContext reductionContext) override;
  LayoutShape leftLayoutShape() const override { return LayoutShape::MoreLetters; };
  LayoutShape rightLayoutShape() const override { return LayoutShape::BoundaryPunctuation; }
  // Evaluation
  Evaluation<float> approximate(SinglePrecision p, ApproximationContext approximationContext) const override {
    return ApproximationHelper::MapOneChild<float>(this, approximationContext, computeOnComplex<float>);
  }
  Evaluation<double> approximate(DoublePrecision p, ApproximationContext approximationContext) const override {
    return ApproximationHelper::MapOneChild<double>(this, approximationContext, computeOnComplex<double>);
  }
};

class ArcSecant final : public Expression {
public:
  ArcSecant(const ArcSecantNode * n) : Expression(n) {}
  static ArcSecant Builder(Expression child) { return TreeHandle::FixedArityBuilder<ArcSecant, ArcSecantNode>({child}); }

  static constexpr Expression::FunctionHelper s_functionHelper = Expression::FunctionHelper("asec", 1, Initializer<ArcSecantNode>, sizeof(ArcSecantNode));

  Expression shallowReduce(ExpressionNode::ReductionContext reductionContext);
};

}

#endif
