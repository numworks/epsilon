#ifndef POINCARE_REEL_PART_H
#define POINCARE_REEL_PART_H

#include <poincare/expression.h>
#include <poincare/approximation_helper.h>

namespace Poincare {

class RealPartNode final : public ExpressionNode  {
public:
  static constexpr char functionName[] = "re";

  // TreeNode
  size_t size() const override { return sizeof(RealPartNode); }
  int numberOfChildren() const override;
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "RealPart";
  }
#endif

  // Properties
  Sign sign(Context * context) const override { return childAtIndex(0)->sign(context); }
  NullStatus nullStatus(Context * context) const override { return childAtIndex(0)->nullStatus(context) == NullStatus::Null ? NullStatus::Null : NullStatus::Unknown; }
  Type type() const override { return Type::RealPart; }


private:
  // Layout
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  // Simplification
  Expression shallowReduce(ReductionContext reductionContext) override;
  LayoutShape leftLayoutShape() const override { return LayoutShape::MoreLetters; };
  LayoutShape rightLayoutShape() const override { return LayoutShape::BoundaryPunctuation; }
  // Evaluation
  template<typename T> static Complex<T> computeOnComplex(const std::complex<T> c, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) {
    return Complex<T>::Builder(std::real(c));
  }
  Evaluation<float> approximate(SinglePrecision p, ApproximationContext approximationContext) const override {
    return ApproximationHelper::MapOneChild<float>(this, approximationContext, computeOnComplex<float>);
  }
  Evaluation<double> approximate(DoublePrecision p, ApproximationContext approximationContext) const override {
    return ApproximationHelper::MapOneChild<double>(this, approximationContext, computeOnComplex<double>);
  }
};

class RealPart final : public HandleOneChild<RealPart, RealPartNode> {
public:
  using Handle::Handle, Handle::Builder, Handle::s_functionHelper;
  Expression shallowReduce(ExpressionNode::ReductionContext reductionContext);
};

}

#endif
