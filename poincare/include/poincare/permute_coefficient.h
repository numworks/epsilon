#ifndef POINCARE_PERMUTE_COEFFICIENT_H
#define POINCARE_PERMUTE_COEFFICIENT_H

#include <poincare/expression.h>
#include <poincare/evaluation.h>

namespace Poincare {

class PermuteCoefficientNode final : public ExpressionNode {
public:

  // TreeNode
  size_t size() const override { return sizeof(PermuteCoefficientNode); }
  int numberOfChildren() const override;
#if POINCARE_TREE_LOG
  virtual void logNodeName(std::ostream & stream) const override {
    stream << "PermuteCoefficient";
  }
#endif

  // ExpressionNode

  // Properties
  Type type() const override{ return Type::PermuteCoefficient; }

  // Complex
  bool isReal(Context & context) const override { return true; }

private:
  // Layout
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  // Simplification
  Expression shallowReduce(Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ReductionTarget target) override;
  // Evaluation
  Evaluation<float> approximate(SinglePrecision p, Context& context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const override { return templatedApproximate<float>(context, complexFormat, angleUnit); }
  Evaluation<double> approximate(DoublePrecision p, Context& context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const override { return templatedApproximate<double>(context, complexFormat, angleUnit); }
  template<typename T> Evaluation<T> templatedApproximate(Context& context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const;
};

class PermuteCoefficient final : public Expression {
public:
  PermuteCoefficient(const PermuteCoefficientNode * n) : Expression(n) {}
  static PermuteCoefficient Builder(Expression child0, Expression child1) { return TreeHandle::FixedArityBuilder<PermuteCoefficient, PermuteCoefficientNode>(ArrayBuilder<TreeHandle>(child0, child1).array(), 2); }
  static constexpr Expression::FunctionHelper s_functionHelper = Expression::FunctionHelper("permute", 2, &UntypedBuilderTwoChildren<PermuteCoefficient>);

  // Expression
  Expression shallowReduce();

  constexpr static int k_maxNValue = 100;
};

}

#endif
