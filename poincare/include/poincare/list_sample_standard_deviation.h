#ifndef POINCARE_LIST_SAMPLE_STANDARD_DEVIATION_H
#define POINCARE_LIST_SAMPLE_STANDARD_DEVIATION_H

#include <poincare/expression.h>

namespace Poincare {

class ListSampleStandardDeviationNode : public ExpressionNode {
public:
  size_t size() const override { return sizeof(ListSampleStandardDeviationNode); }
  int numberOfChildren() const override;
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "ListSampleStandardDeviation";
  }
#endif
  Type type() const override { return Type::ListSampleStandardDeviation; }
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

class ListSampleStandardDeviation : public Expression {
public:
  static constexpr FunctionHelper s_functionHelper = FunctionHelper("samplestddev", 1, &UntypedBuilderOneChild<ListSampleStandardDeviation>);

  ListSampleStandardDeviation(const ListSampleStandardDeviationNode * n) : Expression(n) {}
  static ListSampleStandardDeviation Builder(Expression list) { return TreeHandle::FixedArityBuilder<ListSampleStandardDeviation, ListSampleStandardDeviationNode>({list}); }

  Expression shallowReduce(ExpressionNode::ReductionContext reductionContext);
};

}

#endif
