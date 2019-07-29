#ifndef POINCARE_FACTOR_H
#define POINCARE_FACTOR_H

#include <poincare/expression.h>
#include <poincare/rational.h>
#include <poincare/multiplication_explicite.h>
#include <cmath>

namespace Poincare {

class FactorNode /*final*/ : public ExpressionNode {
public:
  // TreeNode
  size_t size() const override { return sizeof(FactorNode); }
  int numberOfChildren() const override;
#if POINCARE_TREE_LOG
  virtual void logNodeName(std::ostream & stream) const override {
    stream << "Factor";
  }
#endif
  Type type() const override { return Type::Factor; }
private:
  /* Layout */
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  /* Serialization */
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  /* Simplification */
  Expression shallowBeautify(ReductionContext reductionContext) override;
  Expression shallowReduce(ReductionContext reductionContext) override;
  LayoutShape leftLayoutShape() const override { return LayoutShape::DigitOrLetter; };
  LayoutShape rightLayoutShape() const override { return LayoutShape::BoundaryPunctuation; }

  /* Evaluation */
  Evaluation<float> approximate(SinglePrecision p, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const override { return templatedApproximate<float>(context, complexFormat, angleUnit); }
  Evaluation<double> approximate(DoublePrecision p, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const override { return templatedApproximate<double>(context, complexFormat, angleUnit); }
  template<typename T> Evaluation<T> templatedApproximate(Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const {
    return childAtIndex(0)->approximate(T(), context, complexFormat, angleUnit);
  }
};

class Factor final : public Expression {
public:
  Factor(const FactorNode * n) : Expression(n) {}
  static Factor Builder(Expression child) { return TreeHandle::FixedArityBuilder<Factor, FactorNode>(&child, 1); }

  static constexpr Expression::FunctionHelper s_functionHelper = Expression::FunctionHelper("factor", 1, &UntypedBuilderOneChild<Factor>);

  MultiplicationExplicite createMultiplicationOfIntegerPrimeDecomposition(Integer i, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const;

  // Expression
  Expression shallowReduce(Context * context);
  Expression shallowBeautify(ExpressionNode::ReductionContext reductionContext);
};

}
#endif
