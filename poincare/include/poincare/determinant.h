#ifndef POINCARE_DETERMINANT_H
#define POINCARE_DETERMINANT_H

#include <poincare/expression.h>

namespace Poincare {

class DeterminantNode /*final*/ : public ExpressionNode {
public:

  // TreeNode
  size_t size() const override { return sizeof(DeterminantNode); }
  int numberOfChildren() const override;
#if POINCARE_TREE_LOG
  virtual void logNodeName(std::ostream & stream) const override {
    stream << "Determinant";
  }
#endif

  Type type() const override { return Type::Determinant; }
private:
  /* Layout */
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  /* Serialization */
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  /* Simplification */
  Expression shallowReduce(Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ReductionTarget target, bool symbolicComputation) override;
  /* Approximation */
  Evaluation<float> approximate(SinglePrecision p, Context& context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const override { return templatedApproximate<float>(context, complexFormat, angleUnit); }
  Evaluation<double> approximate(DoublePrecision p, Context& context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const override { return templatedApproximate<double>(context, complexFormat, angleUnit); }
 template<typename T> Evaluation<T> templatedApproximate(Context& context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const;
};


class Determinant final : public Expression {
public:
  Determinant(const DeterminantNode * n) : Expression(n) {}
  static Determinant Builder(Expression child) { return TreeHandle::FixedArityBuilder<Determinant, DeterminantNode>(&child, 1); }

  static constexpr Expression::FunctionHelper s_functionHelper = Expression::FunctionHelper("det", 1, &UntypedBuilderOneChild<Determinant>);

  Expression shallowReduce(Context & context);
};

}

#endif
