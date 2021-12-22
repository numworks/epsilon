#ifndef POINCARE_LIST_MEDIAN_H
#define POINCARE_LIST_MEDIAN_H

#include <poincare/expression.h>

namespace Poincare {

class ListMedianNode : public ExpressionNode {
public:
  size_t size() const override { return sizeof(ListMedianNode); }
  int numberOfChildren() const override;
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "ListMedian";
  }
#endif
  Type type() const override { return Type::ListMedian; }
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

class ListMedian : public Expression {
friend class ListMedianNode;
public:
  static constexpr FunctionHelper s_functionHelper = FunctionHelper("med", 1, &UntypedBuilderOneChild<ListMedian>);

  ListMedian(const ListMedianNode * n) : Expression(n) {}
  static ListMedian Builder(Expression list) { return TreeHandle::FixedArityBuilder<ListMedian, ListMedianNode>({list}); }

private:
  Expression shallowReduce(ExpressionNode::ReductionContext reductionContext);
  void approximationHelper(int * index1, int * index2, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const;
};

}

#endif
