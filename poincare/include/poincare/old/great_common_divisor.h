#ifndef POINCARE_GREAT_COMMON_DIVISOR_H
#define POINCARE_GREAT_COMMON_DIVISOR_H

#include <limits.h>

#include "n_ary_expression.h"

namespace Poincare {

class GreatCommonDivisorNode final : public NAryExpressionNode {
 public:
  // PoolObject
  size_t size() const override { return sizeof(GreatCommonDivisorNode); }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream& stream) const override {
    stream << "GreatCommonDivisor";
  }
#endif

  // ExpressionNode
  OMG::Troolean isPositive(Context* context) const override {
    return OMG::Troolean::True;
  }
  Type otype() const override { return Type::GreatCommonDivisor; }

 private:
  // Layout
  size_t serialize(char* buffer, size_t bufferSize,
                   Preferences::PrintFloatMode floatDisplayMode,
                   int numberOfSignificantDigits) const override;
  // Simplification
  OExpression shallowBeautify(
      const ReductionContext& reductionContext) override;
  OExpression shallowReduce(const ReductionContext& reductionContext) override;
  LayoutShape leftLayoutShape() const override {
    return LayoutShape::MoreLetters;
  };
  LayoutShape rightLayoutShape() const override {
    return LayoutShape::BoundaryPunctuation;
  }
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
  template <typename T>
  Evaluation<T> templatedApproximate(
      const ApproximationContext& approximationContext) const;
};

class GreatCommonDivisor final : public NAryExpression {
 public:
  GreatCommonDivisor(const GreatCommonDivisorNode* n) : NAryExpression(n) {}
  static GreatCommonDivisor Builder(const Tuple& children = {}) {
    return PoolHandle::NAryBuilder<GreatCommonDivisor, GreatCommonDivisorNode>(
        convert(children));
  }
  constexpr static OExpression::FunctionHelper s_functionHelper =
      OExpression::FunctionHelper(
          "gcd", 2, INT_MAX,
          &UntypedBuilderMultipleChildren<GreatCommonDivisor>);

  // OExpression
  OExpression shallowReduce(ReductionContext reductionContext);
  OExpression shallowBeautify(Context* context);
};

}  // namespace Poincare

#endif
