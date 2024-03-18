#ifndef POINCARE_LIST
#define POINCARE_LIST

#include <poincare/expression.h>

namespace Poincare {

class ListNode : public ExpressionNode {
 public:
  friend class List;
  // TreeNode
  size_t size() const override { return sizeof(ListNode); }
  int numberOfChildren() const override { return m_numberOfChildren; }
  void setNumberOfChildren(int numberOfChildren) override {
    m_numberOfChildren = numberOfChildren;
  }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream& stream) const override { stream << "List"; }
#endif

  // Properties
  Type type() const override { return Type::List; }

  // Layout
  size_t serialize(char* buffer, size_t bufferSize,
                   Preferences::PrintFloatMode floatDisplayMode,
                   int numberOfSignificantDigits) const override;
  LayoutShape leftLayoutShape() const override { return LayoutShape::Brace; };

  // Simplification
  Expression shallowReduce(const ReductionContext& reductionContext) override;

  // Evaluation
  Evaluation<float> approximate(
      SinglePrecision p,
      const ApproximationContext& approximationContext) const override {
    return templatedApproximate<float>(approximationContext);
  }
  Evaluation<double> approximate(
      DoublePrecision p,
      const ApproximationContext& approximationContext) const override {
    return templatedApproximate<double>(approximationContext);
  }

  // Helper functions
  int extremumIndex(const ApproximationContext& approximationContext,
                    bool minimum);
  template <typename T>
  Evaluation<T> extremumApproximation(
      const ApproximationContext& approximationContext, bool minimum);
  template <typename T>
  Evaluation<T> sumOfElements(const ApproximationContext& approximationContext);
  template <typename T>
  Evaluation<T> productOfElements(
      const ApproximationContext& approximationContext);

 private:
  template <typename T>
  Evaluation<T> templatedApproximate(
      const ApproximationContext& approximationContext) const {
    return templatedApproximate<T>(approximationContext, true);
  }
  template <typename T>
  Evaluation<T> templatedApproximate(
      const ApproximationContext& approximationContext, bool keepUndef) const;

  /* See comment on NAryExpressionNode */
  uint16_t m_numberOfChildren;
};

class List : public Expression {
  friend class ListNode;

 public:
  static List Builder() { return TreeHandle::NAryBuilder<List, ListNode>(); }
  static Expression Ones(int length);

  List() : Expression() {}
  List(const ListNode* n) : Expression(n) {}

  using TreeHandle::addChildAtIndexInPlace;
  using TreeHandle::removeChildAtIndexInPlace;

  ListNode* node() const { return static_cast<ListNode*>(Expression::node()); }
  Expression extremum(const ReductionContext& reductionContext, bool minimum);
  Expression shallowReduce(ReductionContext reductionContext);
  bool isListOfPoints(Context* context) const;
  template <typename T>
  Expression approximateAndRemoveUndefAndSort(
      const ApproximationContext& approximationContext) const;
};

}  // namespace Poincare

#endif
