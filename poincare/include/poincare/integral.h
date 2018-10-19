#ifndef POINCARE_INTEGRAL_H
#define POINCARE_INTEGRAL_H

#include <poincare/expression.h>

namespace Poincare {

class IntegralNode final : public ExpressionNode {
public:

  // TreeNode
  size_t size() const override { return sizeof(IntegralNode); }
  int numberOfChildren() const override { return 3; }
#if POINCARE_TREE_LOG
  virtual void logNodeName(std::ostream & stream) const override {
    stream << "Integral";
  }
#endif

  // ExpressionNode
  Type type() const override { return Type::Integral; }
  int polynomialDegree(char symbolName) const override;
private:
  // Layout
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  // Simplification
  Expression shallowReduce(Context & context, Preferences::AngleUnit angleUnit) override;
  // Evaluation
  Evaluation<float> approximate(SinglePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override { return templatedApproximate<float>(context, angleUnit); }
  Evaluation<double> approximate(DoublePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override { return templatedApproximate<double>(context, angleUnit); }
 template<typename T> Evaluation<T> templatedApproximate(Context& context, Preferences::AngleUnit angleUnit) const;
  template<typename T>
  struct DetailedResult
  {
    T integral;
    T absoluteError;
  };
  constexpr static int k_maxNumberOfIterations = 10;
#ifdef LAGRANGE_METHOD
  template<typename T> T lagrangeGaussQuadrature(T a, T b, Context & context, Preferences::AngleUnit angleUnit) const;
#else
  template<typename T> DetailedResult<T> kronrodGaussQuadrature(T a, T b, Context & context, Preferences::AngleUnit angleUnit) const;
  template<typename T> T adaptiveQuadrature(T a, T b, T eps, int numberOfIterations, Context & context, Preferences::AngleUnit angleUnit) const;
#endif
  template<typename T> T functionValueAtAbscissa(T x, Context & xcontext, Preferences::AngleUnit angleUnit) const;
};

class Integral final : public Expression {
public:
  Integral();
  Integral(const IntegralNode * n) : Expression(n) {}
  Integral(Expression child1, Expression child2, Expression child3) : Integral() {
    replaceChildAtIndexInPlace(0, child1);
    replaceChildAtIndexInPlace(1, child2);
    replaceChildAtIndexInPlace(2, child3);
  }

  // Expression
  Expression shallowReduce(Context & context, Preferences::AngleUnit angleUnit);
};

}

#endif
