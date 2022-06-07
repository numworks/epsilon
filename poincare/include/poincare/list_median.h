#ifndef POINCARE_LIST_MEDIAN_H
#define POINCARE_LIST_MEDIAN_H

#include <poincare/expression.h>

namespace Poincare {

template<int U>
class ListMedianNode : public ExpressionNode {
public:
  size_t size() const override { return sizeof(ListMedianNode); }
  int numberOfChildren() const override { return U; }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "ListMedian";
  }
  void logAttributes(std::ostream & stream) const override {
    stream << " numberOfParameters=\"" << U << "\"";
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
friend class ListMedianNode<1>;
friend class ListMedianNode<2>;
public:
  constexpr static const char * k_functionName = "med";
  static constexpr FunctionHelper s_functionHelperOneChild = FunctionHelper(k_functionName, 1, &Initializer<ListMedianNode<1>>, sizeof(ListMedianNode<1>));
  static constexpr FunctionHelper s_functionHelperTwoChildren = FunctionHelper(k_functionName, 2, &Initializer<ListMedianNode<2>>, sizeof(ListMedianNode<2>));

  ListMedian(const ListMedianNode<1> * n) : Expression(n) {}
  ListMedian(const ListMedianNode<2> * n) : Expression(n) {}
  static ListMedian Builder(Expression list) { return TreeHandle::FixedArityBuilder<ListMedian, ListMedianNode<1>>({list}); }
  static ListMedian Builder(Expression values, Expression weights) { return TreeHandle::FixedArityBuilder<ListMedian, ListMedianNode<2>>({values, weights}); }

private:
  Expression shallowReduce(ExpressionNode::ReductionContext reductionContext);
  void approximationHelper(int * index1, int * index2, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const;
};

}

#endif
