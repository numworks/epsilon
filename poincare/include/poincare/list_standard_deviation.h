#ifndef POINCARE_LIST_STANDARD_DEVIATION_H
#define POINCARE_LIST_STANDARD_DEVIATION_H

#include <poincare/expression.h>

namespace Poincare {

template<int U>
class ListStandardDeviationNode : public ExpressionNode {
public:
  size_t size() const override { return sizeof(ListStandardDeviationNode); }
  int numberOfChildren() const override { return U; }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "ListStandardDeviation";
  }
  void logAttributes(std::ostream & stream) const override {
    stream << " numberOfParameters=\"" << U << "\"";
  }
#endif
  Type type() const override { return Type::ListStandardDeviation; }
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

class ListStandardDeviation : public Expression {
public:
  constexpr static const char * k_functionName = "stddev";
  constexpr static FunctionHelper s_functionHelperOneChild = FunctionHelper(k_functionName, 1, &Initializer<ListStandardDeviationNode<1>>, sizeof(ListStandardDeviationNode<1>));
  constexpr static FunctionHelper s_functionHelperTwoChildren = FunctionHelper(k_functionName, 2, &Initializer<ListStandardDeviationNode<2>>, sizeof(ListStandardDeviationNode<2>));

  ListStandardDeviation(const ListStandardDeviationNode<1> * n) : Expression(n) {}
  ListStandardDeviation(const ListStandardDeviationNode<2> * n) : Expression(n) {}
  static ListStandardDeviation Builder(Expression list) { return TreeHandle::FixedArityBuilder<ListStandardDeviation, ListStandardDeviationNode<1>>({list}); }
    static ListStandardDeviation Builder(Expression values, Expression weights) { return TreeHandle::FixedArityBuilder<ListStandardDeviation, ListStandardDeviationNode<2>>({values, weights}); }

  Expression shallowReduce(ExpressionNode::ReductionContext reductionContext);
};

}

#endif
