#ifndef POINCARE_CONJUGATE_H
#define POINCARE_CONJUGATE_H

#include <poincare/approximation_helper.h>
#include <poincare/expression.h>

namespace Poincare {

class ConjugateNode final : public ExpressionNode  {
public:
  constexpr static AliasesList k_functionName = "conj";

  // TreeNode
  size_t size() const override { return sizeof(ConjugateNode); }
  int numberOfChildren() const override;
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "Conjugate";
  }
#endif

  // Properties
  TrinaryBoolean isPositive(Context * context) const override { return childAtIndex(0)->isPositive(context); }
  TrinaryBoolean isNull(Context * context) const override { return childAtIndex(0)->isNull(context); }
  Type type() const override { return Type::Conjugate; }
private:
  // Layout
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits, Context * context) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  // Simplification
  Expression shallowReduce(const ReductionContext& reductionContext) override;
  LayoutShape leftLayoutShape() const override { return childAtIndex(0)->leftLayoutShape(); };
  LayoutShape rightLayoutShape() const override { return childAtIndex(0)->rightLayoutShape(); }

  // Evaluation
  template<typename T> static Complex<T> computeOnComplex(const std::complex<T> c, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit);
  Evaluation<float> approximate(SinglePrecision p, const ApproximationContext& approximationContext) const override {
    return ApproximationHelper::MapOneChild<float>(this, approximationContext, computeOnComplex<float>);
  }
  Evaluation<double> approximate(DoublePrecision p, const ApproximationContext& approximationContext) const override {
    return ApproximationHelper::MapOneChild<double>(this, approximationContext, computeOnComplex<double>);
  }
};

class Conjugate final : public ExpressionOneChild<Conjugate, ConjugateNode> {
public:
  using ExpressionBuilder::ExpressionBuilder;
  Expression shallowReduce(ReductionContext reductionContext);
};

}

#endif
