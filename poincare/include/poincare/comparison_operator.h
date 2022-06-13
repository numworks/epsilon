#ifndef POINCARE_COMPARISON_OPERATOR_H
#define POINCARE_COMPARISON_OPERATOR_H

#include <poincare/expression.h>

namespace Poincare {

class ComparisonOperatorNode : public ExpressionNode {
public:
  // TreeNode
  int numberOfChildren() const override { return 2; }

  // ExpressionNode
  int polynomialDegree(Context * context, const char * symbolName) const override { return -1; }
protected:
  virtual const char * comparisonString() const = 0;
  virtual CodePoint comparisonCodePoint() const = 0;
private:
  LayoutShape leftLayoutShape() const override { assert(false); return LayoutShape::BoundaryPunctuation; };
  // Layout
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  // Evaluation
  Evaluation<float> approximate(SinglePrecision p, const ApproximationContext& approximationContext) const override;
  Evaluation<double> approximate(DoublePrecision p, const ApproximationContext& approximationContext) const override;
  template<typename T> Evaluation<T> templatedApproximate(const ApproximationContext& approximationContext) const;
};

class ComparisonOperator : public Expression {
public:
  ComparisonOperator(const ComparisonOperatorNode * n) : Expression(n) {}
  static bool IsComparisonOperatorType(ExpressionNode::Type type);
  static ExpressionNode::Type Opposite(ExpressionNode::Type type);
  // For the comparison A ? B, create the reduced expression A-B
  Expression standardEquation(Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, Preferences::UnitFormat unitFormat, ExpressionNode::ReductionTarget reductionTarget) const;
};

}

#endif
