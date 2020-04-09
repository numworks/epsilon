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
    stream << "UnivtConvert";
  }
#endif
  // ExpressionNode
  Type type() const override { return Type::UnitConvert; }

private:
  Expression getUnit() const override { assert(false); return ExpressionNode::getUnit(); }
  // Simplification
  Expression shallowReduce(ReductionContext reductionContext) override;
  // Evalutation
  Evaluation<float> approximate(SinglePrecision p, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const override { return templatedApproximate<float>(context, complexFormat, angleUnit); }
  Evaluation<double> approximate(DoublePrecision p, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const override { return templatedApproximate<double>(context, complexFormat, angleUnit); }
  template<typename T> Evaluation<T> templatedApproximate(Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const;
};

class UnitConvert final : public Expression {
friend class UnitConvertNode;
public:
  UnitConvert(const UnitConvertNode * n) : Expression(n) {}
  static UnitConvert Builder(Expression value, Expression unit) { return TreeHandle::FixedArityBuilder<UnitConvert, UnitConvertNode>({value, unit}); }

  // Expression
  Expression shallowReduce(ExpressionNode::ReductionContext reductionContext);

private:
  UnitConvertNode * node() const { return static_cast<UnitConvertNode *>(Expression::node()); }
};

}

#endif
