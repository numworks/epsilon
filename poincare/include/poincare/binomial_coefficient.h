#ifndef POINCARE_BINOMIAL_COEFFICIENT_H
#define POINCARE_BINOMIAL_COEFFICIENT_H

#include <poincare/approximation_helper.h>
#include <poincare/expression.h>

namespace Poincare {

class BinomialCoefficientNode final : public ExpressionNode {
public:

  // TreeNode
  size_t size() const override { return sizeof(BinomialCoefficientNode); }
  int numberOfChildren() const override;
#if POINCARE_TREE_LOG
  virtual void logNodeName(std::ostream & stream) const override {
    stream << "BinomialCoefficient";
  }
#endif

  // Complex
  bool isReal(Context & context) const override { return true; }

  // Properties
  Type type() const override{ return Type::BinomialCoefficient; }
  template<typename T> static T compute(T k, T n);
private:
  // Layout
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  // Simplification
  Expression shallowReduce(Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ReductionTarget target, bool symbolicComputation) override;
  // Evaluation
  Evaluation<float> approximate(SinglePrecision p, Context& context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const override { return templatedApproximate<float>(context, complexFormat, angleUnit); }
  Evaluation<double> approximate(DoublePrecision p, Context& context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const override { return templatedApproximate<double>(context, complexFormat, angleUnit); }
  template<typename T> Complex<T> templatedApproximate(Context& context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const;
};

class BinomialCoefficient final : public Expression {
public:
  BinomialCoefficient(const BinomialCoefficientNode * n) : Expression(n) {}
  static BinomialCoefficient Builder(Expression child0, Expression child1) { return TreeHandle::FixedArityBuilder<BinomialCoefficient, BinomialCoefficientNode>(ArrayBuilder<TreeHandle>(child0, child1).array(), 2); }
  static constexpr Expression::FunctionHelper s_functionHelper = Expression::FunctionHelper("binomial", 2, &UntypedBuilderTwoChildren<BinomialCoefficient>);

  // Expression
  Expression shallowReduce();
private:
  constexpr static int k_maxNValue = 300;
};

}

#endif
