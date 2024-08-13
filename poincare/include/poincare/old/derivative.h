#ifndef POINCARE_DERIVATIVE_H
#define POINCARE_DERIVATIVE_H

#include "parametered_expression.h"
#include "rational.h"
#include "symbol.h"

namespace Poincare {

class DerivativeNode final : public ParameteredExpressionNode {
  friend class Derivative;

 public:
  // PoolObject
  size_t size() const override { return sizeof(DerivativeNode); }
  int numberOfChildren() const override;
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream& stream) const override {
    stream << "Derivative";
  }
#endif

  // Properties
  Type otype() const override { return Type::Derivative; }

 private:
  bool isFirstOrder() const {
    return OExpression(childAtIndex(numberOfChildren() - 1)).isOne();
  }
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

  bool isValidCondensedForm() const;
  Expression createValidExpandedForm() const;
  int extractIntegerOrder() const;
};

class Derivative final : public ParameteredExpression {
 public:
  Derivative(const DerivativeNode* n) : ParameteredExpression(n) {}

  static Derivative Builder(OExpression child0, Symbol child1,
                            OExpression child2) {
    return PoolHandle::FixedArityBuilder<Derivative, DerivativeNode>(
        {child0, child1, child2, Rational::Builder(1)});
  }
  static Derivative Builder(OExpression child0, Symbol child1,
                            OExpression child2, OExpression child3) {
    return PoolHandle::FixedArityBuilder<Derivative, DerivativeNode>(
        {child0, child1, child2, child3});
  }
  static OExpression UntypedBuilder(OExpression children);
  constexpr static OExpression::FunctionHelper s_functionHelper =
      OExpression::FunctionHelper("diff", 4, &UntypedBuilder);

  constexpr static OExpression::FunctionHelper s_functionHelperFirstOrder =
      OExpression::FunctionHelper("diff", 3, &UntypedBuilder);

  static void DerivateUnaryFunction(OExpression function, Symbol symbol,
                                    OExpression symbolValue,
                                    const ReductionContext& reductionContext);
  static OExpression DefaultDerivate(OExpression function,
                                     const ReductionContext& reductionContext,
                                     Symbol symbol);

  void deepReduceChildren(const ReductionContext& reductionContext);
  OExpression shallowReduce(ReductionContext reductionContext);

  OExpression distributeOverPoint();
  OExpression createValidExpandedForm() const;
  bool isValidCondensedForm() const {
    return static_cast<DerivativeNode*>(node())->isValidCondensedForm();
  }
};

}  // namespace Poincare

#endif
