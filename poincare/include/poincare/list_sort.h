#ifndef POINCARE_LIST_SORT_H
#define POINCARE_LIST_SORT_H

#include <poincare/expression.h>

namespace Poincare {

class ListSortNode : public ExpressionNode {
public:
  size_t size() const override { return sizeof(ListSortNode); }
  int numberOfChildren() const override;
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "ListSort";
  }
#endif
  Type type() const override { return Type::ListSort; }
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

class ListSort : public Expression {
public:
  constexpr static bool k_nanIsGreatest = true;
  static constexpr FunctionHelper s_functionHelper = FunctionHelper("sort", 1, &Initializer<ListSortNode>, sizeof(ListSortNode));

  ListSort(const ListSortNode * n) : Expression(n) {}
  static ListSort Builder(Expression list) { return TreeHandle::FixedArityBuilder<ListSort, ListSortNode>({list}); }

  Expression shallowReduce(ExpressionNode::ReductionContext reductionContext);
};

}

#endif
