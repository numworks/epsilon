#ifndef POINCARE_LEAST_COMMON_MULTIPLE_H
#define POINCARE_LEAST_COMMON_MULTIPLE_H

#include <poincare/serialization_helper.h>
#include <poincare/expression.h>

namespace Poincare {

class LeastCommonMultipleNode final : public ExpressionNode  {
public:

  // TreeNode
  size_t size() const override { return sizeof(LeastCommonMultipleNode); }
  int numberOfChildren() const override { return 2; }
#if POINCARE_TREE_LOG
  virtual void logNodeName(std::ostream & stream) const override {
    stream << "LeastCommonMultiple";
  }
#endif
  // ExpressionNode
  Type type() const override { return Type::LeastCommonMultiple; }
private:
  /* Layout */
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override {
    return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, name());
  }
  const char * name() const { return "lcm"; }
  /* Simplification */
  Expression shallowReduce(Context & context, Preferences::AngleUnit angleUnit) override;
  /* Evaluation */
  Evaluation<float> approximate(SinglePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override { return templatedApproximate<float>(context, angleUnit); }
  Evaluation<double> approximate(DoublePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override { return templatedApproximate<double>(context, angleUnit); }
  template<typename T> Evaluation<T> templatedApproximate(Context& context, Preferences::AngleUnit angleUnit) const;
};

class LeastCommonMultiple final : public Expression {
public:
  LeastCommonMultiple();
  LeastCommonMultiple(const LeastCommonMultipleNode * n) : Expression(n) {}
  LeastCommonMultiple(Expression child1, Expression child2) : LeastCommonMultiple() {
    replaceChildAtIndexInPlace(0, child1);
    replaceChildAtIndexInPlace(1, child2);
  }

  // Expression
  Expression shallowReduce(Context & context, Preferences::AngleUnit angleUnit);
};

}

#endif
