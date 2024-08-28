#ifndef POINCARE_LEAST_COMMON_MULTIPLE_H
#define POINCARE_LEAST_COMMON_MULTIPLE_H

#include <limits.h>

#include "n_ary_expression.h"

namespace Poincare {

class LeastCommonMultipleNode final : public NAryExpressionNode {
 public:
  // PoolObject
  size_t size() const override { return sizeof(LeastCommonMultipleNode); }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream& stream) const override {
    stream << "LeastCommonMultiple";
  }
#endif

  // ExpressionNode
  OMG::Troolean isPositive(Context* context) const override {
    return OMG::Troolean::True;
  }
  Type otype() const override { return Type::LeastCommonMultiple; }

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
};

class LeastCommonMultiple final : public NAryExpression {
 public:
  LeastCommonMultiple(const LeastCommonMultipleNode* n) : NAryExpression(n) {}
  static LeastCommonMultiple Builder(const Tuple& children = {}) {
    return PoolHandle::NAryBuilder<LeastCommonMultiple,
                                   LeastCommonMultipleNode>(convert(children));
  }
  constexpr static OExpression::FunctionHelper s_functionHelper =
      OExpression::FunctionHelper(
          "lcm", 2, INT_MAX,
          &UntypedBuilderMultipleChildren<LeastCommonMultiple>);

  // OExpression
  OExpression shallowReduce(ReductionContext reductionContext);
  OExpression shallowBeautify(Context* context);
};

}  // namespace Poincare

#endif
