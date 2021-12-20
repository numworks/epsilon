#ifndef POINCARE_LIST_ELEMENT_H
#define POINCARE_LIST_ELEMENT_H

#include <poincare/expression.h>
#include <poincare/symbol.h>

namespace Poincare {

class ListElementNode : public ExpressionNode {
public:
  static constexpr int k_indexChildIndex = 0;
  static constexpr int k_listChildIndex = 1;

  size_t size() const override { return sizeof(ListElementNode); }
  int numberOfChildren() const override { return 2; }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "ListElement";
  }
#endif
  Type type() const override { return Type::ListElement; }
  LayoutShape leftLayoutShape() const override { return LayoutShape::BoundaryPunctuation; };

private:
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;

  Expression shallowReduce(ReductionContext reductionContext) override;

  Evaluation<float> approximate(SinglePrecision p, ApproximationContext approximationContext) const override { return templatedApproximate<float>(approximationContext); }
  Evaluation<double> approximate(DoublePrecision p, ApproximationContext approximationContext) const override { return templatedApproximate<double>(approximationContext); }
  template<typename T> Evaluation<T> templatedApproximate(ApproximationContext approximationContext) const;

  SymbolNode * symbolChild() const { assert(childAtIndex(k_listChildIndex)->type() == Type::Symbol); return static_cast<SymbolNode *>(childAtIndex(k_listChildIndex)); }
};

class ListElement : public Expression {
public:
  ListElement(const ListElementNode * n) : Expression(n) {}
  static ListElement Builder(Expression index, Expression list) { return TreeHandle::FixedArityBuilder<ListElement, ListElementNode>({index, list}); }

  Expression shallowReduce(ExpressionNode::ReductionContext reductionContext);
};

}

#endif
