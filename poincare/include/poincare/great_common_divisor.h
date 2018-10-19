#ifndef POINCARE_GREAT_COMMON_DIVISOR_H
#define POINCARE_GREAT_COMMON_DIVISOR_H

#include <poincare/expression.h>

namespace Poincare {

class GreatCommonDivisorNode final : public ExpressionNode {
public:

  // TreeNode
  size_t size() const override { return sizeof(GreatCommonDivisorNode); }
  int numberOfChildren() const override { return 2; }
#if POINCARE_TREE_LOG
  virtual void logNodeName(std::ostream & stream) const override {
    stream << "GreatCommonDivisor";
  }
#endif

  // ExpressionNode
  Type type() const override { return Type::GreatCommonDivisor; }
private:
  // Layout
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  const char * name() const { return "gcd"; }
  // Simplification
  Expression shallowReduce(Context & context, Preferences::AngleUnit angleUnit) override;
  // Evaluation
  Evaluation<float> approximate(SinglePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override { return templatedApproximate<float>(context, angleUnit); }
  Evaluation<double> approximate(DoublePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override { return templatedApproximate<double>(context, angleUnit); }
  template<typename T> Evaluation<T> templatedApproximate(Context& context, Preferences::AngleUnit angleUnit) const;
};

class GreatCommonDivisor final : public Expression {
public:
  GreatCommonDivisor();
  GreatCommonDivisor(const GreatCommonDivisorNode * n) : Expression(n) {}
  GreatCommonDivisor(Expression child1, Expression child2) : GreatCommonDivisor() {
    replaceChildAtIndexInPlace(0, child1);
    replaceChildAtIndexInPlace(1, child2);
  }

  // Expression
  Expression shallowReduce(Context & context, Preferences::AngleUnit angleUnit);
};

}

#endif
