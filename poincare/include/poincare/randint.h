#ifndef POINCARE_RANDINT_H
#define POINCARE_RANDINT_H

#include <poincare/expression.h>
#include <poincare/default_parameter.h>
#include <poincare/rational.h>

namespace Poincare {

class RandintNode /*final*/ : public ExpressionNode  {
  friend class Randint;
public:
  constexpr static char k_functionName[] = "randint";

  // TreeNode
  size_t size() const override { return sizeof(RandintNode); }
  int numberOfChildren() const override;
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "Randint";
  }
#endif

  // Properties
  Type type() const override { return Type::Randint; }

  // ExpressionNode
  Expression defaultParameterAtIndex(int i) const override {
    return i == 0 ? Rational::Builder(0) : ExpressionNode::defaultParameterAtIndex(i);
  }

private:
  // Layout
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  // Simplification
  Expression shallowReduce(const ReductionContext& reductionContext) override;
  // Evaluation
  Evaluation<float> approximate(SinglePrecision p, const ApproximationContext& approximationContext) const override {
    return templateApproximate<float>(approximationContext);
  }
  Evaluation<double> approximate(DoublePrecision p, const ApproximationContext& approximationContext) const override {
    return templateApproximate<double>(approximationContext);
  }
  template <typename T> Evaluation<T> templateApproximate(const ApproximationContext& approximationContext, bool * inputIsUndefined = nullptr) const;

  LayoutShape leftLayoutShape() const override { return LayoutShape::MoreLetters; };
  LayoutShape rightLayoutShape() const override { return LayoutShape::BoundaryPunctuation; }
};

class Randint final : public ExpressionTwoChildren<Randint, RandintNode> {
friend class RandintNode;
public:
  using ExpressionBuilder::ExpressionBuilder;
  static Expression OneChildUntypedBuilder(Expression children) {
    assert(children.numberOfChildren() == 1);
    return Builder(DefaultParameter::Builder(), children.childAtIndex(0));
  }
  Expression shallowReduce(const ExpressionNode::ReductionContext& reductionContext);

  constexpr static Expression::FunctionHelper s_functionHelperOneChild = Expression::FunctionHelper(RandintNode::k_functionName, 1, &OneChildUntypedBuilder);

};


}

#endif
