#ifndef POINCARE_STORE_H
#define POINCARE_STORE_H

#include <poincare/evaluation.h>
#include <poincare/rightwards_arrow_expression.h>
#include <poincare/symbol_abstract.h>

namespace Poincare {

class StoreNode final : public RightwardsArrowExpressionNode {
 public:
  // TreeNode
  size_t size() const override { return sizeof(StoreNode); }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream& stream) const override { stream << "Store"; }
#endif
  // ExpressionNode
  Type type() const override { return Type::Store; }

 private:
  // Simplification
  Expression shallowReduce(const ReductionContext& reductionContext) override;
  Expression deepReplaceReplaceableSymbols(
      Context* context, TrinaryBoolean* isCircular,
      int parameteredAncestorsCount,
      SymbolicComputation symbolicComputation) override;
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

class Store final : public ExpressionTwoChildren<Store, StoreNode> {
  friend class StoreNode;

 public:
  using ExpressionBuilder::ExpressionBuilder;

  // Store
  const SymbolAbstract symbol() const {
    assert(childAtIndex(1).isOfType(
        {ExpressionNode::Type::Symbol, ExpressionNode::Type::Function}));
    return childAtIndex(1).convert<const SymbolAbstract>();
  }
  const Expression value() const { return childAtIndex(0); }
  bool isTrulyReducedInShallowReduce() const {
    return symbol().type() == ExpressionNode::Type::Symbol;
  }

  // Expression
  void deepReduceChildren(const ReductionContext& reductionContext);
  Expression shallowReduce(ReductionContext reductionContext);
  bool storeValueForSymbol(Context* context) const;
  Expression deepReplaceReplaceableSymbols(
      Context* context, TrinaryBoolean* isCircular,
      int parameteredAncestorsCount, SymbolicComputation symbolicComputation);

  bool storeRecursivelyMatches(
      ExpressionTrinaryTest test, Context* context,
      SymbolicComputation replaceSymbols, void* auxiliary,
      Expression::IgnoredSymbols* ignoredSymbols) const {
    return value().recursivelyMatches(test, context, replaceSymbols, auxiliary,
                                      ignoredSymbols);
  }

 private:
  StoreNode* node() const {
    return static_cast<StoreNode*>(Expression::node());
  }
};

}  // namespace Poincare

#endif
