#ifndef POINCARE_RANDINT_H
#define POINCARE_RANDINT_H

#include <poincare/expression.h>
#include <poincare/rational.h>

namespace Poincare {

class RandintNode /*final*/ : public ExpressionNode  {
  friend class Randint;
public:
  constexpr static char k_functionName[] = "randint";

  // TreeNode
  size_t size() const override { return sizeof(RandintNode); }
  int numberOfChildren() const override { return m_hasTwoChildren ? 2 : 1; }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "Randint";
  }
#endif

  // Properties
  Type type() const override { return Type::Randint; }

  void setNumberOfChildren(int numberOfChildren) override {
    assert(numberOfChildren >= 1 && numberOfChildren <= 2);
    m_hasTwoChildren = numberOfChildren == 2;
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
  bool m_hasTwoChildren;
};

class Randint final : public ExpressionBuilder<Randint,RandintNode,1,2> {
friend class RandintNode;
public:
  using ExpressionBuilder::ExpressionBuilder;
  Expression shallowReduce(const ExpressionNode::ReductionContext& reductionContext);
};


}

#endif
