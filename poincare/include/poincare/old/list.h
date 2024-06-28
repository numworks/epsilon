#ifndef POINCARE_LIST
#define POINCARE_LIST

#include "old_expression.h"

namespace Poincare {

class ListNode : public ExpressionNode {
 public:
  friend class OList;
  // PoolObject
  size_t size() const override { return sizeof(ListNode); }
  int numberOfChildren() const override { return m_numberOfChildren; }
  void setNumberOfChildren(int numberOfChildren) override {
    m_numberOfChildren = numberOfChildren;
  }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream& stream) const override { stream << "OList"; }
#endif

  // Properties
  Type otype() const override { return Type::OList; }

  // Layout
  size_t serialize(char* buffer, size_t bufferSize,
                   Preferences::PrintFloatMode floatDisplayMode,
                   int numberOfSignificantDigits) const override;
  LayoutShape leftLayoutShape() const override { return LayoutShape::Brace; };

  // Simplification
  OExpression shallowReduce(const ReductionContext& reductionContext) override;

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

class OList : public OExpression {
  friend class ListNode;

 public:
  static OList Builder() { return PoolHandle::NAryBuilder<OList, ListNode>(); }
  static OExpression Ones(int length);

  OList() : OExpression() {}
  OList(const ListNode* n) : OExpression(n) {}

  using PoolHandle::addChildAtIndexInPlace;
  using PoolHandle::removeChildAtIndexInPlace;

  ListNode* node() const { return static_cast<ListNode*>(OExpression::node()); }
  OExpression extremum(const ReductionContext& reductionContext, bool minimum);
  OExpression shallowReduce(ReductionContext reductionContext);
};

}  // namespace Poincare

#endif
