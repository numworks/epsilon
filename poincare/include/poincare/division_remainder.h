#ifndef POINCARE_DIVISION_REMAINDER_H
#define POINCARE_DIVISION_REMAINDER_H

#include <poincare/approximation_helper.h>
#include <poincare/expression.h>
#include <poincare/integer.h>

namespace Poincare {

class DivisionRemainderNode final : public ExpressionNode {
public:
  constexpr static AliasesList k_functionName = "rem";

  // TreeNode
  size_t size() const override { return sizeof(DivisionRemainderNode); }
  int numberOfChildren() const override;
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "DivisionRemainder";
  }
#endif

  // ExpressionNode
  Sign sign(Context * context) const override { return Sign::Positive; }
  Type type() const override { return Type::DivisionRemainder; }


  // Simplification
  LayoutShape leftLayoutShape() const override { return LayoutShape::MoreLetters; };
  LayoutShape rightLayoutShape() const override { return LayoutShape::BoundaryPunctuation; }

private:
  // Layout
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits, Context * context) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  // Simplification
  Expression shallowReduce(const ReductionContext& reductionContext) override;
  // Evaluation
  Evaluation<float> approximate(SinglePrecision p, const ApproximationContext& approximationContext) const override { return templatedApproximate<float>(approximationContext); }
  Evaluation<double> approximate(DoublePrecision p, const ApproximationContext& approximationContext) const override { return templatedApproximate<double>(approximationContext); }
  template<typename T> Evaluation<T> templatedApproximate(const ApproximationContext& approximationContext) const;
};

class DivisionRemainder final : public ExpressionTwoChildren<DivisionRemainder, DivisionRemainderNode> {
public:
  using ExpressionBuilder::ExpressionBuilder;
  // Expression
  Expression shallowReduce(ExpressionNode::ReductionContext reductionContext);
  static Expression Reduce(const Integer & a, const Integer & b);
};

}

#endif
