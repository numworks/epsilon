#ifndef POINCARE_STORE_H
#define POINCARE_STORE_H

#include "evaluation.h"
#include "rightwards_arrow_expression.h"
#include "symbol_abstract.h"

namespace Poincare {

class StoreNode final : public RightwardsArrowExpressionNode {
 public:
  // PoolObject
  size_t size() const override { return sizeof(StoreNode); }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream& stream) const override { stream << "Store"; }
#endif
  // ExpressionNode
  Type otype() const override { return Type::Store; }

 private:
  // Simplification
  OExpression shallowReduce(const ReductionContext& reductionContext) override;
  // Evalutation
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
  template <typename T>
  Evaluation<T> templatedApproximate(
      const ApproximationContext& approximationContext) const;
};

class OStore final : public ExpressionTwoChildren<OStore, StoreNode> {
  friend class StoreNode;

 public:
  using ExpressionBuilder::ExpressionBuilder;

  // Store
  const SymbolAbstract symbol() const {
    assert(childAtIndex(1).isOfType(
        {ExpressionNode::Type::Symbol, ExpressionNode::Type::Function}));
    return childAtIndex(1).convert<const SymbolAbstract>();
  }
  const OExpression value() const { return childAtIndex(0); }
  bool isTrulyReducedInShallowReduce() const {
    return symbol().otype() == ExpressionNode::Type::Symbol;
  }

  // OExpression
  void deepReduceChildren(const ReductionContext& reductionContext);
  OExpression shallowReduce(ReductionContext reductionContext);

 private:
  StoreNode* node() const {
    return static_cast<StoreNode*>(OExpression::node());
  }
};

}  // namespace Poincare

#endif
