#ifndef POINCARE_LIST
#define POINCARE_LIST

#include <poincare/expression.h>

namespace Poincare {

class ListNode : public ExpressionNode {
public:
  // TreeNode
  size_t size() const override { return sizeof(ListNode); }
  int numberOfChildren() const override { return m_numberOfChildren; }
  void incrementNumberOfChildren(int increment = 1) override { m_numberOfChildren+= increment; }
  void decrementNumberOfChildren(int decrement = 1) override {
    assert(m_numberOfChildren >= decrement);
    m_numberOfChildren-= decrement;
  }
  void eraseNumberOfChildren() override { m_numberOfChildren = 0; }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "List";
  }
#endif

  // Properties
  Type type() const override { return Type::List; }

  // Layout
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  LayoutShape leftLayoutShape() const override { return LayoutShape::BoundaryPunctuation; };

  // Simplification
  Expression shallowReduce(ReductionContext reductionContext) override;

  // Evaluation
  Evaluation<float> approximate(SinglePrecision p, ApproximationContext approximationContext) const override { return templatedApproximate<float>(approximationContext); }
  Evaluation<double> approximate(DoublePrecision p, ApproximationContext approximationContext) const override { return templatedApproximate<double>(approximationContext); }

private:
  template<typename T> Evaluation<T> templatedApproximate(ApproximationContext approximationContext) const;

  /* See comment on NAryExpressionNode */
  uint16_t m_numberOfChildren;
};

class List : public Expression {
  friend class ListNode;
public:
  List(const ListNode * n) : Expression(n) {}
  static List Builder() { return TreeHandle::NAryBuilder<List, ListNode>(); }
  using TreeHandle::addChildAtIndexInPlace;
};

}

#endif
