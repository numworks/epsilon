#ifndef POINCARE_UNIT_CONVERT_H
#define POINCARE_UNIT_CONVERT_H

#include <poincare/expression.h>
#include <poincare/unit.h>
#include <poincare/evaluation.h>

namespace Poincare {

class UnitConvertNode /*final*/ : public ExpressionNode {
public:

  // TreeNode
  size_t size() const override { return sizeof(UnitConvertNode); }
  int numberOfChildren() const override { return 2; }
#if POINCARE_TREE_LOG
  virtual void logNodeName(std::ostream & stream) const override {
    stream << "UnivtConvert";
  }
#endif

  // ExpressionNode
  Type type() const override { return Type::UnitConvert; }
  int polynomialDegree(Context * context, const char * symbolName) const override { return -1; }

private:
  // Simplification
  void deepReduceChildren(ExpressionNode::ReductionContext reductionContext) override {}
  Expression shallowReduce(ReductionContext reductionContext) override;
  LayoutShape leftLayoutShape() const override { assert(false); return LayoutShape::MoreLetters; };
  // Layout
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  // Evalutation
  Evaluation<float> approximate(SinglePrecision p, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const override { return templatedApproximate<float>(context, complexFormat, angleUnit); }
  Evaluation<double> approximate(DoublePrecision p, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const override { return templatedApproximate<double>(context, complexFormat, angleUnit); }
  template<typename T> Evaluation<T> templatedApproximate(Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const;
};

class UnitConvert final : public Expression {
friend class UnitConvertNode;
public:
  UnitConvert(const UnitConvertNode * n) : Expression(n) {}
  static UnitConvert Builder(Expression value, Expression unit) { return TreeHandle::FixedArityBuilder<UnitConvert, UnitConvertNode>(ArrayBuilder<TreeHandle>(value, unit).array(), 2); }

  // Expression
  Expression shallowReduce(ExpressionNode::ReductionContext reductionContext);

private:
  UnitConvertNode * node() const { return static_cast<UnitConvertNode *>(Expression::node()); }
};

}

#endif
