#ifndef POINCARE_CONJUGATE_H
#define POINCARE_CONJUGATE_H

#include "approximation_helper.h"
#include "old_expression.h"

namespace Poincare {

class ConjugateNode final : public ExpressionNode {
 public:
  constexpr static AliasesList k_functionName = "conj";

  // PoolObject
  size_t size() const override { return sizeof(ConjugateNode); }
  int numberOfChildren() const override;
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream& stream) const override {
    stream << "Conjugate";
  }
#endif

  // Properties
  OMG::Troolean isPositive(Context* context) const override {
    return childAtIndex(0)->isPositive(context);
  }
  OMG::Troolean isNull(Context* context) const override {
    return childAtIndex(0)->isNull(context);
  }
  Type otype() const override { return Type::Conjugate; }

 private:
  // Layout
  size_t serialize(char* buffer, size_t bufferSize,
                   Preferences::PrintFloatMode floatDisplayMode,
                   int numberOfSignificantDigits) const override;
  // Simplification
  OExpression shallowReduce(const ReductionContext& reductionContext) override;
  LayoutShape leftLayoutShape() const override {
    return childAtIndex(0)->leftLayoutShape();
  };
  LayoutShape rightLayoutShape() const override {
    return childAtIndex(0)->rightLayoutShape();
  }
};

class Conjugate final : public ExpressionOneChild<Conjugate, ConjugateNode> {
 public:
  using ExpressionBuilder::ExpressionBuilder;
  OExpression shallowReduce(ReductionContext reductionContext);
};

}  // namespace Poincare

#endif
