#ifndef POINCARE_INTEGRAL_H
#define POINCARE_INTEGRAL_H

#include <poincare/expression.h>
#include <poincare/symbol.h>

namespace Poincare {

class IntegralNode final : public ExpressionNode {
public:

  // TreeNode
  size_t size() const override { return sizeof(IntegralNode); }
  int numberOfChildren() const override;
#if POINCARE_TREE_LOG
  virtual void logNodeName(std::ostream & stream) const override {
    stream << "Integral";
  }
#endif

  // ExpressionNode
  Type type() const override { return Type::Integral; }
  int polynomialDegree(Context & context, const char * symbolName) const override;
  Expression replaceUnknown(const Symbol & symbol) override;

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
  template<typename T>
  struct DetailedResult
  {
    T integral;
    T absoluteError;
  };
  constexpr static int k_maxNumberOfIterations = 10;
#ifdef LAGRANGE_METHOD
  template<typename T> T lagrangeGaussQuadrature(T a, T b, Context Context & context, Preferences::AngleUnit angleUnit context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const;
#else
  template<typename T> DetailedResult<T> kronrodGaussQuadrature(T a, T b, Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const;
  template<typename T> T adaptiveQuadrature(T a, T b, T eps, int numberOfIterations, Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const;
#endif
  template<typename T> T functionValueAtAbscissa(T x, Context & xcontext, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const;
};

class Integral final : public Expression {
public:
  Integral(const IntegralNode * n) : Expression(n) {}
  static Integral Builder(Expression child0, Symbol child1, Expression child2, Expression child3) { return TreeHandle::FixedArityBuilder<Integral, IntegralNode>(ArrayBuilder<TreeHandle>(child0, child1, child2, child3).array(), 4); }
  static Expression UntypedBuilder(Expression children);

  static constexpr Expression::FunctionHelper s_functionHelper = Expression::FunctionHelper("int", 4, &UntypedBuilder);

  // Expression
  Expression shallowReduce();
};

}

#endif
