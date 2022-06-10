#ifndef POINCARE_ABSOLUTE_VALUE_H
#define POINCARE_ABSOLUTE_VALUE_H

#include <poincare/expression.h>
#include <poincare/rational.h>
#include <poincare/approximation_helper.h>

namespace Poincare {

class AbsoluteValueNode final : public ExpressionNode {
public:
  static constexpr char k_functionName[] = "abs";

  // TreeNode
  size_t size() const override { return sizeof(AbsoluteValueNode); }
  int numberOfChildren() const override;
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "AbsoluteValue";
  }
#endif

  // Properties
  Type type() const override { return Type::AbsoluteValue; }
  Sign sign(Context * context) const override { return Sign::Positive; }
  NullStatus nullStatus(Context * context) const override { return childAtIndex(0)->nullStatus(context); }

  // Approximation
  template<typename T> static Complex<T> computeOnComplex(const std::complex<T> c, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) {
    return Complex<T>::Builder(std::abs(c));
  }
  Evaluation<float> approximate(SinglePrecision p, ApproximationContext approximationContext) const override {
    return ApproximationHelper::MapOneChild<float>(this, approximationContext, computeOnComplex<float>);
  }
  Evaluation<double> approximate(DoublePrecision p, ApproximationContext approximationContext) const override {
    return ApproximationHelper::MapOneChild<double>(this, approximationContext, computeOnComplex<double>);
  }

  // Layout
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;

  // Simplification
  Expression shallowReduce(ReductionContext reductionContext) override;
  LayoutShape leftLayoutShape() const override { return LayoutShape::BoundaryPunctuation; }
private:
  bool derivate(ReductionContext reductionContext, Symbol symbol, Expression symbolValue) override;
};

class AbsoluteValue final : public HandleOneChild<AbsoluteValue, AbsoluteValueNode> {
friend class AbsoluteValueNode;
public:
  using Handle::Handle, Handle::Builder, Handle::s_functionHelper;
  Expression shallowReduce(ExpressionNode::ReductionContext reductionContext);
  bool derivate(ExpressionNode::ReductionContext reductionContext, Symbol symbol, Expression symbolValue);
};

}

#endif
