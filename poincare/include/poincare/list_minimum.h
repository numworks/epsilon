#ifndef POINCARE_LIST_MINIMUM_H
#define POINCARE_LIST_MINIMUM_H

#include <poincare/list.h>

namespace Poincare {

class ListMinimumNode : public ExpressionNode {
public:
  size_t size() const override { return sizeof(ListMinimumNode); }
  int numberOfChildren() const override;
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "ListMinimum";
  }
#endif
  Type type() const override { return Type::ListMinimum; }
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

class ListMinimum : public Expression {
public:
  static constexpr FunctionHelper s_functionHelper = FunctionHelper("min", 1, &UntypedBuilderOneChild<ListMinimum>);
  ListMinimum(const ListMinimumNode * n) : Expression(n) {}
  static ListMinimum Builder(Expression list) { return TreeHandle::FixedArityBuilder<ListMinimum, ListMinimumNode>({list}); }

  Expression shallowReduce(ExpressionNode::ReductionContext reductionContext);
};

}

#endif
