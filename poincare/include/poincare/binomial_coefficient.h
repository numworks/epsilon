#ifndef POINCARE_BINOMIAL_COEFFICIENT_H
#define POINCARE_BINOMIAL_COEFFICIENT_H

#include <poincare/approximation_helper.h>
#include <poincare/expression.h>
#include <poincare/layout_helper.h>

namespace Poincare {

class BinomialCoefficientNode final : public ExpressionNode {
public:

  // TreeNode
  size_t size() const override { return sizeof(BinomialCoefficientNode); }
  int numberOfChildren() const override { return 2; }
#if POINCARE_TREE_LOG
  virtual void logNodeName(std::ostream & stream) const override {
    stream << "BinomialCoefficient";
  }
#endif

  // ExpressionNode

  // Properties
  Type type() const override{ return Type::BinomialCoefficient; }
  template<typename T> static T compute(T k, T n);
private:
  // Layout
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  // Simplification
  Expression shallowReduce(Context & context, Preferences::AngleUnit angleUnit) override;
  // Evaluation
  Evaluation<float> approximate(SinglePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override { return templatedApproximate<float>(context, angleUnit); }
  Evaluation<double> approximate(DoublePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override { return templatedApproximate<double>(context, angleUnit); }
  template<typename T> Complex<T> templatedApproximate(Context& context, Preferences::AngleUnit angleUnit) const;
};

class BinomialCoefficient final : public Expression {
public:
  BinomialCoefficient();
  BinomialCoefficient(const BinomialCoefficientNode * n) : Expression(n) {}
  BinomialCoefficient(Expression child1, Expression child2) : BinomialCoefficient() {
    replaceChildAtIndexInPlace(0, child1);
    replaceChildAtIndexInPlace(1, child2);
  }

  // Expression
  Expression shallowReduce(Context & context, Preferences::AngleUnit angleUnit);
private:
  constexpr static int k_maxNValue = 300;
};

}

#endif
