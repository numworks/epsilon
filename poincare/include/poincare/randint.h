#ifndef POINCARE_RANDINT_H
#define POINCARE_RANDINT_H

#include <poincare/expression.h>

namespace Poincare {

class RandintNode /*final*/ : public ExpressionNode  {
public:

  // TreeNode
  size_t size() const override { return sizeof(RandintNode); }
  int numberOfChildren() const override;
#if POINCARE_TREE_LOG
  virtual void logNodeName(std::ostream & stream) const override {
    stream << "Randint";
  }
#endif

  // Properties
  Type type() const override { return Type::Randint; }

  // Complex
  bool isReal(Context & context) const override { return true; }

private:
  // Layout
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  // Evaluation
  Evaluation<float> approximate(SinglePrecision p, Context& context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const override {
    return templateApproximate<float>(context, complexFormat, angleUnit);
  }
  Evaluation<double> approximate(DoublePrecision p, Context& context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const override {
    return templateApproximate<double>(context, complexFormat, angleUnit);
  }
  template <typename T> Evaluation<T> templateApproximate(Context& context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const;
};

class Randint final : public Expression {
friend class RandintNode;
public:
  Randint(const RandintNode * n) : Expression(n) {}
  static Randint Builder(Expression child0, Expression child1);
  static Expression UntypedBuilder(Expression children) { return Builder(children.childAtIndex(0), children.childAtIndex(1)); }
  static constexpr Expression::FunctionHelper s_functionHelper = Expression::FunctionHelper("randint", 2, &UntypedBuilder);
};

}

#endif
