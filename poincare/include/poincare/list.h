#ifndef POINCARE_LIST
#define POINCARE_LIST

#include <poincare/expression.h>

namespace Poincare {

class ListNode : public ExpressionNode {
public:
  // TreeNode
  size_t size() const override { return sizeof(ListNode); }
  int numberOfChildren() const override { return m_numberOfChildren; }
  void setNumberOfChildren(int numberOfChildren) override { m_numberOfChildren = numberOfChildren; }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "List";
  }
#endif

  // Properties
  Type type() const override { return Type::List; }

  // Layout
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits, Context * context) const override;
  LayoutShape leftLayoutShape() const override { return LayoutShape::Default; };
  LayoutShape rightLayoutShape() const override { return LayoutShape::Brace; };

  // Simplification
  Expression shallowReduce(const ReductionContext& reductionContext) override;

  // Evaluation
  Evaluation<float> approximate(SinglePrecision p, const ApproximationContext& approximationContext) const override { return templatedApproximate<float>(approximationContext); }
  Evaluation<double> approximate(DoublePrecision p, const ApproximationContext& approximationContext) const override { return templatedApproximate<double>(approximationContext); }

  // Helper functions
  int extremumIndex(const ApproximationContext& approximationContext, bool minimum, bool returnMinusOneIfCantApproximate);
  template <typename T> Evaluation<T> extremumApproximation(const ApproximationContext& approximationContext, bool minimum);
  template<typename T> Evaluation<T> sumOfElements(const ApproximationContext& approximationContext);
  template<typename T> Evaluation<T> productOfElements(const ApproximationContext& approximationContext);

private:
  template<typename T> Evaluation<T> templatedApproximate(const ApproximationContext& approximationContext) const;

  /* See comment on NAryExpressionNode */
  uint16_t m_numberOfChildren;
};

class List : public Expression {
  friend class ListNode;
public:
  List() : Expression() {}
  List(const ListNode * n) : Expression(n) {}
  static List Builder() { return TreeHandle::NAryBuilder<List, ListNode>(); }
  static Expression Ones(int length);
  ListNode * node() const { return static_cast<ListNode *>(Expression::node()); }
  Expression extremum(const ExpressionNode::ReductionContext& reductionContext, bool minimum);
  Expression shallowReduce(ExpressionNode::ReductionContext reductionContext);
  using TreeHandle::addChildAtIndexInPlace;
  using TreeHandle::removeChildAtIndexInPlace;
};

}

#endif
