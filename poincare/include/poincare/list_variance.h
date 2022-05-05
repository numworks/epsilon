#ifndef POINCARE_LIST_VARIANCE_H
#define POINCARE_LIST_VARIANCE_H

#include <poincare/list.h>

namespace Poincare {

class ListVarianceNode : public ExpressionNode {
public:
  size_t size() const override { return sizeof(ListVarianceNode); }
  int numberOfChildren() const override;
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "ListVariance";
  }
#endif
  Type type() const override { return Type::ListVariance; }
  LayoutShape leftLayoutShape() const override { return LayoutShape::MoreLetters; };
  LayoutShape rightLayoutShape() const override { return LayoutShape::BoundaryPunctuation; }

private:
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;

  Expression shallowReduce(ReductionContext reductionContext) override;

  Evaluation<float> approximate(SinglePrecision p, ApproximationContext approximationContext) const override { return templatedApproximate<float>(approximationContext); }
  Evaluation<double> approximate(DoublePrecision p, ApproximationContext approximationContext) const override { return templatedApproximate<double>(approximationContext); }
  template<typename T> Evaluation<T> templatedApproximate(ApproximationContext approximationContext) const;
};

class ListVariance : public Expression {
public:
  static constexpr FunctionHelper s_functionHelper = FunctionHelper("var", 1, &UntypedBuilderOneChild<ListVariance>);

  ListVariance(const ListVarianceNode * n) : Expression(n) {}
  static ListVariance Builder(Expression list) { return TreeHandle::FixedArityBuilder<ListVariance, ListVarianceNode>({list}); }

  Expression shallowReduce(ExpressionNode::ReductionContext reductionContext);
};

}

#endif
