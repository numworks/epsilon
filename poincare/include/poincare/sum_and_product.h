#ifndef POINCARE_SUM_AND_PRODUCT_H
#define POINCARE_SUM_AND_PRODUCT_H

#include <poincare/parametered_expression.h>
#include <poincare/symbol.h>
#include <poincare/approximation_helper.h>

namespace Poincare {

class SumAndProductNode : public ParameteredExpressionNode {
public:
  int numberOfChildren() const override { return 4; }
private:
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits, Context * context) const override;
  virtual Layout createSumAndProductLayout(Layout argumentLayout, Layout symbolLayout, Layout subscriptLayout, Layout superscriptLayout) const = 0;
  // Simplication
  Expression shallowReduce(const ReductionContext& reductionContext) override;
  LayoutShape leftLayoutShape() const override { return LayoutShape::BoundaryPunctuation; };
  /* Approximation */
  Evaluation<float> approximate(SinglePrecision p, const ApproximationContext& approximationContext) const override { return templatedApproximate<float>(approximationContext); }
  Evaluation<double> approximate(DoublePrecision p, const ApproximationContext& approximationContext) const override { return templatedApproximate<double>(approximationContext); }
 template<typename T> Evaluation<T> templatedApproximate(const ApproximationContext& approximationContext) const;
  virtual float emptySumAndProductValue() const = 0;
  virtual Evaluation<float> evaluateWithNextTerm(SinglePrecision p, Evaluation<float> a, Evaluation<float> b, Preferences::ComplexFormat complexFormat) const = 0;
  virtual Evaluation<double> evaluateWithNextTerm(DoublePrecision p, Evaluation<double> a, Evaluation<double> b, Preferences::ComplexFormat complexFormat) const = 0;
};

class SumAndProduct : public Expression {
public:
  SumAndProduct(const SumAndProductNode * n) : Expression(n) {}
  Expression shallowReduce(ReductionContext reductionContext);
  constexpr static char k_defaultXNTChar = 'k';
};

}

#endif
