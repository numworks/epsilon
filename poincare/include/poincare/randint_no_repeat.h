#ifndef POINCARE_RANDINT_NO_REPEAT_H
#define POINCARE_RANDINT_NO_REPEAT_H

#include <poincare/expression.h>

namespace Poincare {

class RandintNoRepeatNode : public ExpressionNode {
public:
  constexpr static AliasesList k_functionName = "randintnorep";

  size_t size() const override { return sizeof(RandintNoRepeatNode); }
  int numberOfChildren() const override;
  Type type() const override { return Type::RandintNoRepeat; }
  LayoutShape leftLayoutShape() const override { return LayoutShape::MoreLetters; };
  LayoutShape rightLayoutShape() const override { return LayoutShape::BoundaryPunctuation; }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "RandintNoRepeat";
  }
#endif


private:
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits, Context * context) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  Expression shallowReduce(const ReductionContext& reductionContext) override;
  Evaluation<float> approximate(SinglePrecision p, const ApproximationContext& approximationContext) const override { return templatedApproximate<float>(approximationContext); }
  Evaluation<double> approximate(DoublePrecision p, const ApproximationContext& approximationContext) const override { return templatedApproximate<double>(approximationContext); }
  template<typename T> Evaluation<T> templatedApproximate(const ApproximationContext& approximationContext) const;
};

class RandintNoRepeat : public ExpressionThreeChildren<RandintNoRepeat, RandintNoRepeatNode> {
public:
  using ExpressionBuilder::ExpressionBuilder;

  Expression shallowReduce(ExpressionNode::ReductionContext reductionContext);
};

}

#endif
