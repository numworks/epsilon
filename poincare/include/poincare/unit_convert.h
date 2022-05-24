#ifndef POINCARE_UNIT_CONVERT_H
#define POINCARE_UNIT_CONVERT_H

#include <poincare/rightwards_arrow_expression.h>
#include <poincare/unit.h>
#include <poincare/evaluation.h>

namespace Poincare {

class UnitConvertNode /*final*/ : public RightwardsArrowExpressionNode {
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
  void deepReduceChildren(ExpressionNode::ReductionContext reductionContext) override;
  Expression deepBeautify(ExpressionNode::ReductionContext reductionContext) override;
  Expression shallowBeautify(ReductionContext * reductionContext) override;
  // Evalutation
  Evaluation<float> approximate(SinglePrecision p, ApproximationContext approximationContext) const override { return templatedApproximate<float>(approximationContext); }
  Evaluation<double> approximate(DoublePrecision p, ApproximationContext approximationContext) const override { return templatedApproximate<double>(approximationContext); }
  template<typename T> Evaluation<T> templatedApproximate(ApproximationContext approximationContext) const;
};

class UnitConvert final : public Expression {
friend class UnitConvertNode;
public:
  UnitConvert(const UnitConvertNode * n) : Expression(n) {}
  static UnitConvert Builder(Expression value, Expression unit) { return TreeHandle::FixedArityBuilder<UnitConvert, UnitConvertNode>({value, unit}); }

  // Expression
  void deepReduceChildren(ExpressionNode::ReductionContext reductionContext);
  Expression deepBeautify(ExpressionNode::ReductionContext reductionContext);
  Expression shallowBeautify(ExpressionNode::ReductionContext * reductionContext);
private:
  UnitConvertNode * node() const { return static_cast<UnitConvertNode *>(Expression::node()); }
};

}

#endif
