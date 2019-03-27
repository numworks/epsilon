#ifndef POINCARE_DERIVATIVE_H
#define POINCARE_DERIVATIVE_H

#include <poincare/expression.h>
#include <poincare/symbol.h>
#include <poincare/variable_context.h>

namespace Poincare {

class DerivativeNode final : public ExpressionNode {
public:

  // TreeNode
  size_t size() const override { return sizeof(DerivativeNode); }
  int numberOfChildren() const override;
#if POINCARE_TREE_LOG
  virtual void logNodeName(std::ostream & stream) const override {
    stream << "Derivative";
  }
#endif

  // Complex
  bool isReal(Context & context) const override { return true; }

  // Properties
  Type type() const override { return Type::Derivative; }
  int polynomialDegree(Context & context, const char * symbolName) const override;
  Expression replaceUnknown(const Symbol & symbol, const Symbol & unknownSymbol) override;

private:
  // Layout
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;

  // Simplification
  Expression shallowReduce(Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ReductionTarget target, bool symbolicComputation) override;

  // Evaluation
  Evaluation<float> approximate(SinglePrecision p, Context& context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const override { return templatedApproximate<float>(context, complexFormat, angleUnit); }
  Evaluation<double> approximate(DoublePrecision p, Context& context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const override { return templatedApproximate<double>(context, complexFormat, angleUnit); }
  template<typename T> Evaluation<T> templatedApproximate(Context& context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const;
  template<typename T> T approximateWithArgument(T x, Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const;
  template<typename T> T growthRateAroundAbscissa(T x, T h, Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const;
  template<typename T> T riddersApproximation(Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, T x, T h, T * error) const;
  // TODO: Change coefficients?
  constexpr static double k_maxErrorRateOnApproximation = 0.001;
  constexpr static double k_minInitialRate = 0.01;
  constexpr static double k_rateStepSize = 1.4;
};

class Derivative final : public Expression {
public:
  Derivative(const DerivativeNode * n) : Expression(n) {}
  static Derivative Builder(Expression child0, Symbol child1, Expression child2) { return TreeHandle::FixedArityBuilder<Derivative, DerivativeNode>(ArrayBuilder<TreeHandle>(child0, child1, child2).array(), 3); }
  static Expression UntypedBuilder(Expression children);
  static constexpr Expression::FunctionHelper s_functionHelper = Expression::FunctionHelper("diff", 3, &UntypedBuilder);

  Expression shallowReduce();
};

}

#endif
