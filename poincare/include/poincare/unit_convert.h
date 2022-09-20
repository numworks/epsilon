#ifndef POINCARE_UNIT_CONVERT_H
#define POINCARE_UNIT_CONVERT_H

#include <poincare/rightwards_arrow_expression.h>
#include <poincare/unit.h>
#include <poincare/evaluation.h>

namespace Poincare {

class UnitConvertNode final : public RightwardsArrowExpressionNode {
public:
  // TreeNode
  size_t size() const override { return sizeof(UnitConvertNode); }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "UnitConvert";
  }
#endif
  // ExpressionNode
  Type type() const override { return Type::UnitConvert; }

private:
  Expression removeUnit(Expression * unit) override;
  // Simplification
  Expression shallowBeautify(const ReductionContext& reductionContext) override;
  // Evalutation
  Evaluation<float> approximate(SinglePrecision p, const ApproximationContext& approximationContext) const override { return templatedApproximate<float>(approximationContext); }
  Evaluation<double> approximate(DoublePrecision p, const ApproximationContext& approximationContext) const override { return templatedApproximate<double>(approximationContext); }
  template<typename T> Evaluation<T> templatedApproximate(const ApproximationContext& approximationContext) const;
};

class UnitConvert final : public ExpressionTwoChildren<UnitConvert, UnitConvertNode> {
friend class UnitConvertNode;
public:
  using ExpressionBuilder::ExpressionBuilder;
  // Expression
  void deepReduceChildren(const ExpressionNode::ReductionContext& reductionContext);
  Expression deepBeautify(const ExpressionNode::ReductionContext& reductionContext);
  Expression shallowBeautify(const ExpressionNode::ReductionContext& reductionContext);
private:
  UnitConvertNode * node() const { return static_cast<UnitConvertNode *>(Expression::node()); }
};

}

#endif
