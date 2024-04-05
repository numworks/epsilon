#ifndef POINCARE_UNIT_CONVERT_H
#define POINCARE_UNIT_CONVERT_H

#include <poincare/evaluation.h>
#include <poincare/rightwards_arrow_expression.h>
#include <poincare/unit.h>

namespace Poincare {

class UnitConvertNode final : public RightwardsArrowExpressionNode {
 public:
  // PoolObject
  size_t size() const override { return sizeof(UnitConvertNode); }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream& stream) const override {
    stream << "UnitConvert";
  }
#endif
  // ExpressionNode
  Type otype() const override { return Type::UnitConvert; }

 private:
  OExpression removeUnit(OExpression* unit) override;
  // Simplification
  OExpression shallowBeautify(
      const ReductionContext& reductionContext) override;
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

class UnitConvert final
    : public ExpressionTwoChildren<UnitConvert, UnitConvertNode> {
  friend class UnitConvertNode;

 public:
  using ExpressionBuilder::ExpressionBuilder;
  // OExpression
  void deepReduceChildren(const ReductionContext& reductionContext);
  OExpression deepBeautify(const ReductionContext& reductionContext);
  OExpression shallowBeautify(const ReductionContext& reductionContext);

 private:
  UnitConvertNode* node() const {
    return static_cast<UnitConvertNode*>(OExpression::node());
  }
};

}  // namespace Poincare

#endif
