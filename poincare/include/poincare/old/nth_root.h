#ifndef POINCARE_NTH_ROOT_H
#define POINCARE_NTH_ROOT_H

#include "old_expression.h"

namespace Poincare {

class NthRootNode final : public ExpressionNode {
 public:
  constexpr static AliasesList k_functionName = "root";
  // ExpressionNode
  Type otype() const override { return Type::NthRoot; }

  // PoolObject
  size_t size() const override { return sizeof(NthRootNode); }
  int numberOfChildren() const override;
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream& stream) const override { stream << "NthRoot"; }
#endif

 private:
  OExpression removeUnit(OExpression* unit) override {
    assert(false);
    return ExpressionNode::removeUnit(unit);
  }
  // Layout
  size_t serialize(char* buffer, size_t bufferSize,
                   Preferences::PrintFloatMode floatDisplayMode,
                   int numberOfSignificantDigits) const override;
  // Simplification
  OExpression shallowReduce(const ReductionContext& reductionContext) override;
  LayoutShape leftLayoutShape() const override { return LayoutShape::NthRoot; };
  LayoutShape rightLayoutShape() const override { return LayoutShape::Root; };
};

class NthRoot final : public ExpressionTwoChildren<NthRoot, NthRootNode> {
 public:
  using ExpressionBuilder::ExpressionBuilder;
  OExpression shallowReduce(ReductionContext reductionContext);
};

}  // namespace Poincare

#endif
