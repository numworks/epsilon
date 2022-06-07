#ifndef POINCARE_LIST_PRODUCT_H
#define POINCARE_LIST_PRODUCT_H

#include <poincare/expression.h>

namespace Poincare {

class ListProductNode : public ExpressionNode {
public:
  size_t size() const override { return sizeof(ListProductNode); }
  int numberOfChildren() const override;
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "ListProduct";
  }
#endif
  Type type() const override { return Type::ListProduct; }
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

class ListProduct : public Expression {
public:
  static constexpr FunctionHelper s_functionHelper = FunctionHelper("prod", 1, &Initializer<ListProductNode>, sizeof(ListProductNode));

  ListProduct(const ListProductNode * n) : Expression(n) {}
  static ListProduct Builder(Expression list) { return TreeHandle::FixedArityBuilder<ListProduct, ListProductNode>({list}); }

  Expression shallowReduce(ExpressionNode::ReductionContext reductionContext);
};

}

#endif
