#ifndef POINCARE_SIGN_FUNCTION_H
#define POINCARE_SIGN_FUNCTION_H

#include "approximation_helper.h"
#include "old_expression.h"

namespace Poincare {

class SignFunctionNode final : public ExpressionNode {
 public:
  constexpr static AliasesList k_functionName = "sign";

  // PoolObject
  size_t size() const override { return sizeof(SignFunctionNode); }
  int numberOfChildren() const override;
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream& stream) const override {
    stream << "SignFunction";
  }
#endif

  // Properties
  Type otype() const override { return Type::SignFunction; }
  OMG::Troolean isPositive(Context* context) const override {
    return childAtIndex(0)->isPositive(context);
  }
  OMG::Troolean isNull(Context* context) const override {
    return childAtIndex(0)->isNull(context);
  }

 private:
  // Layout
  size_t serialize(char* buffer, size_t bufferSize,
                   Preferences::PrintFloatMode floatDisplayMode,
                   int numberOfSignificantDigits) const override;
  // Simplification
  OExpression shallowReduce(const ReductionContext& reductionContext) override;
  LayoutShape leftLayoutShape() const override {
    return LayoutShape::MoreLetters;
  };
  LayoutShape rightLayoutShape() const override {
    return LayoutShape::BoundaryPunctuation;
  }

  // Derivation
  bool derivate(const ReductionContext& reductionContext, Symbol symbol,
                OExpression symbolValue) override;
};

class SignFunction final
    : public ExpressionOneChild<SignFunction, SignFunctionNode> {
 public:
  using ExpressionBuilder::ExpressionBuilder;

  OExpression shallowReduce(ReductionContext reductionContext);
  bool derivate(const ReductionContext& reductionContext, Symbol symbol,
                OExpression symbolValue);
};

}  // namespace Poincare

#endif
