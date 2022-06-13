#ifndef POINCARE_LIST_SAMPLE_STANDARD_DEVIATION_H
#define POINCARE_LIST_SAMPLE_STANDARD_DEVIATION_H

#include <poincare/expression.h>

namespace Poincare {

template<int U>
class ListSampleStandardDeviationNode : public ExpressionNode {
public:
  size_t size() const override { return sizeof(ListSampleStandardDeviationNode); }
  int numberOfChildren() const override { return U; }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "ListSampleStandardDeviation";
  }
  void logAttributes(std::ostream & stream) const override {
    stream << " numberOfParameters=\"" << U << "\"";
  }
#endif
  Type type() const override { return Type::ListSampleStandardDeviation; }
  LayoutShape leftLayoutShape() const override { return LayoutShape::MoreLetters; };
  LayoutShape rightLayoutShape() const override { return LayoutShape::BoundaryPunctuation; }

private:
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;

  Expression shallowReduce(const ReductionContext& reductionContext) override;

  Evaluation<float> approximate(SinglePrecision p, const ApproximationContext& approximationContext) const override { return templatedApproximate<float>(approximationContext); }
  Evaluation<double> approximate(DoublePrecision p, const ApproximationContext& approximationContext) const override { return templatedApproximate<double>(approximationContext); }
  template<typename T> Evaluation<T> templatedApproximate(const ApproximationContext& approximationContext) const;
};

class ListSampleStandardDeviation : public Expression {
public:
  constexpr static const char * k_functionName = "samplestddev";
  static constexpr FunctionHelper s_functionHelperOneChild = FunctionHelper(k_functionName, 1, &Initializer<ListSampleStandardDeviationNode<1>>, sizeof(ListSampleStandardDeviationNode<1>));
  static constexpr FunctionHelper s_functionHelperTwoChildren = FunctionHelper(k_functionName, 2, &Initializer<ListSampleStandardDeviationNode<2>>, sizeof(ListSampleStandardDeviationNode<2>));

  ListSampleStandardDeviation(const ListSampleStandardDeviationNode<1> * n) : Expression(n) {}
  ListSampleStandardDeviation(const ListSampleStandardDeviationNode<2> * n) : Expression(n) {}
  static ListSampleStandardDeviation Builder(Expression list) { return TreeHandle::FixedArityBuilder<ListSampleStandardDeviation, ListSampleStandardDeviationNode<1>>({list}); }
  static ListSampleStandardDeviation Builder(Expression values, Expression weights) { return TreeHandle::FixedArityBuilder<ListSampleStandardDeviation, ListSampleStandardDeviationNode<2>>({values, weights}); }

  Expression shallowReduce(const ExpressionNode::ReductionContext& reductionContext);
};

}

#endif
