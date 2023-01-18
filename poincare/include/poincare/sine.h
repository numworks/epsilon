#ifndef POINCARE_SINE_H
#define POINCARE_SINE_H

#include <poincare/approximation_helper.h>
#include <poincare/expression.h>
#include <poincare/trigonometry.h>

namespace Poincare {

class SineNode final : public ExpressionNode {
public:
  constexpr static AliasesList k_functionName = "sin";

  // TreeNode
  size_t size() const override { return sizeof(SineNode); }
  int numberOfChildren() const override;
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "Sine";
  }
#endif

  // Properties
  Type type() const override { return Type::Sine; }

  template<typename T> static Complex<T> computeOnComplex(const std::complex<T> c, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit = Preferences::AngleUnit::Radian);

private:
  // Layout
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits, Context * context) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;

  // Simplication
  Expression shallowReduce(const ReductionContext& reductionContext) override;
  LayoutShape leftLayoutShape() const override { return LayoutShape::MoreLetters; };
  LayoutShape rightLayoutShape() const override { return LayoutShape::BoundaryPunctuation; }

  // Derivation
  bool derivate(const ReductionContext& reductionContext, Symbol symbol, Expression symbolValue) override;
  Expression unaryFunctionDifferential(const ReductionContext& reductionContext) override;

  // Evaluation
  Evaluation<float> approximate(SinglePrecision p, const ApproximationContext& approximationContext) const override {
    return ApproximationHelper::MapOneChild<float>(this, approximationContext, computeOnComplex<float>);
  }
  Evaluation<double> approximate(DoublePrecision p, const ApproximationContext& approximationContext) const override {
    return ApproximationHelper::MapOneChild<double>(this, approximationContext, computeOnComplex<double>);
  }
};

class Sine final : public ExpressionOneChild<Sine, SineNode> {
public:
  using ExpressionBuilder::ExpressionBuilder;

  bool derivate(const ReductionContext& reductionContext, Symbol symbol, Expression symbolValue);
  Expression unaryFunctionDifferential(const ReductionContext& reductionContext);
};

}

#endif
