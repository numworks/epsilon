#ifndef POINCARE_LIST_MEAN_H
#define POINCARE_LIST_MEAN_H

#include <poincare/list.h>

namespace Poincare {

template<int U>
class ListMeanNode : public ExpressionNode {
public:
  size_t size() const override { return sizeof(ListMeanNode<U>); }
  int numberOfChildren() const override { return U; }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "ListMean" ;
  }
  void logAttributes(std::ostream & stream) const override {
    stream << " numberOfParameters=\"" << U << "\"";
  }
#endif
  Type type() const override { return Type::ListMean; }
  LayoutShape leftLayoutShape() const override { return LayoutShape::MoreLetters; };
  LayoutShape rightLayoutShape() const override { return LayoutShape::BoundaryPunctuation; }

private:
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;

  Expression shallowReduce(const ReductionContext& reductionContext) override;

  Evaluation<float> approximate(SinglePrecision p, ApproximationContext approximationContext) const override { return templatedApproximate<float>(approximationContext); }
  Evaluation<double> approximate(DoublePrecision p, ApproximationContext approximationContext) const override { return templatedApproximate<double>(approximationContext); }
  template<typename T> Evaluation<T> templatedApproximate(ApproximationContext approximationContext) const;
};

class ListMean : public Expression {
public:
  constexpr static const char * k_functionName = "mean";
  constexpr static FunctionHelper s_functionHelperOneChild = FunctionHelper(k_functionName, 1, &Initializer<ListMeanNode<1>>, sizeof(ListMeanNode<1>));
  constexpr static FunctionHelper s_functionHelperTwoChildren = FunctionHelper(k_functionName, 2, &Initializer<ListMeanNode<2>>, sizeof(ListMeanNode<2>));

  ListMean(const ListMeanNode<1> * n) : Expression(n) {}
  ListMean(const ListMeanNode<2> * n) : Expression(n) {}
  static ListMean Builder(Expression list) { return TreeHandle::FixedArityBuilder<ListMean, ListMeanNode<1>>({list}); }
  static ListMean Builder(Expression values, Expression weights) { return TreeHandle::FixedArityBuilder<ListMean, ListMeanNode<2>>({values, weights}); }

  Expression shallowReduce(const ExpressionNode::ReductionContext& reductionContext);
};

}

#endif
