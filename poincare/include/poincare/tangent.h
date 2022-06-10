#ifndef POINCARE_TANGENT_H
#define POINCARE_TANGENT_H

#include <poincare/approximation_helper.h>
#include <poincare/expression.h>

namespace Poincare {

class TangentNode final : public ExpressionNode {
public:
  static constexpr char k_functionName[] = "tan";

  // TreeNode
  size_t size() const override { return sizeof(TangentNode); }
  int numberOfChildren() const override;
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "Tangent";
  }
#endif

  // Properties
  Type type() const override { return Type::Tangent; }

private:
  // Layout
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;

  // Simplication
  Expression shallowReduce(ReductionContext reductionContext) override;
  LayoutShape leftLayoutShape() const override { return LayoutShape::MoreLetters; };
  LayoutShape rightLayoutShape() const override { return LayoutShape::BoundaryPunctuation; }

  // Derivation
  bool derivate(ReductionContext reductionContext, Symbol symbol, Expression symbolValue) override;
  Expression unaryFunctionDifferential(ReductionContext reductionContext) override;

  // Evaluation
  template<typename T> static Complex<T> computeOnComplex(const std::complex<T> c, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit = Preferences::AngleUnit::Radian);
  Evaluation<float> approximate(SinglePrecision p, ApproximationContext approximationContext) const override {
    return ApproximationHelper::MapOneChild<float>(this, approximationContext, computeOnComplex<float>);
  }
  Evaluation<double> approximate(DoublePrecision p, ApproximationContext approximationContext) const override {
    return ApproximationHelper::MapOneChild<double>(this, approximationContext, computeOnComplex<double>);
  }
};

class Tangent final : public HandleOneChild<Tangent, TangentNode> {
public:
  using Handle::Handle, Handle::Builder, Handle::s_functionHelper;

  Expression shallowReduce(ExpressionNode::ReductionContext reductionContext);

  bool derivate(ExpressionNode::ReductionContext reductionContext, Symbol symbol, Expression symbolValue);
  Expression unaryFunctionDifferential(ExpressionNode::ReductionContext reductionContext);
};

}

#endif
