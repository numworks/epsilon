#ifndef POINCARE_DIVISION_REMAINDER_H
#define POINCARE_DIVISION_REMAINDER_H

#include <poincare/approximation_helper.h>
#include <poincare/expression.h>

namespace Poincare {

class DivisionRemainderNode final : public ExpressionNode {
public:

  // TreeNode
  size_t size() const override { return sizeof(DivisionRemainderNode); }
  int numberOfChildren() const override;
#if POINCARE_TREE_LOG
  virtual void logNodeName(std::ostream & stream) const override {
    stream << "DivisionRemainder";
  }
#endif

  // ExpressionNode
  Type type() const override { return Type::DivisionRemainder; }

  // Complex
  bool isReal(Context * context) const override { return true; }

private:
  // Layout
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  // Simplification
  Expression shallowReduce(ReductionContext reductionContext) override;
  // Evaluation
  Evaluation<float> approximate(SinglePrecision p, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const override { return templatedApproximate<float>(context, complexFormat, angleUnit); }
  Evaluation<double> approximate(DoublePrecision p, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const override { return templatedApproximate<double>(context, complexFormat, angleUnit); }
  template<typename T> Evaluation<T> templatedApproximate(Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const;
};

class DivisionRemainder final : public Expression {
public:
  DivisionRemainder(const DivisionRemainderNode * n) : Expression(n) {}
  static DivisionRemainder Builder(Expression child0, Expression child1) { return TreeHandle::FixedArityBuilder<DivisionRemainder, DivisionRemainderNode>(ArrayBuilder<TreeHandle>(child0, child1).array(), 2); }
  static constexpr Expression::FunctionHelper s_functionHelper = Expression::FunctionHelper("rem", 2, &UntypedBuilderTwoChildren<DivisionRemainder>);

  // Expression
  Expression shallowReduce();
};

}

#endif
