#ifndef POINCARE_COMPARISON_OPERATOR_H
#define POINCARE_COMPARISON_OPERATOR_H

#include <poincare/expression.h>
#include <poincare/evaluation.h>

namespace Poincare {

class ComparisonOperatorNode : public ExpressionNode {
public:
  // TreeNode
  int numberOfChildren() const override { return 2; }

  // ExpressionNode
  int polynomialDegree(Context * context, const char * symbolName) const override { return -1; }
  Sign sign(Context * context) const override { return Sign::Positive; }
protected:
  virtual const char * comparisonString() const = 0;
  virtual CodePoint comparisonCodePoint() const = 0;
private:
  // Layout
  LayoutShape leftLayoutShape() const override { assert(false); return LayoutShape::BoundaryPunctuation; };
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits, Context * context) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;

  // Evaluation
  Evaluation<float> approximate(SinglePrecision p, const ApproximationContext& approximationContext) const override;
  Evaluation<double> approximate(DoublePrecision p, const ApproximationContext& approximationContext) const override;
  template<typename T> Evaluation<T> templatedApproximate(const ApproximationContext& approximationContext) const;

  // Simplification
  Expression shallowReduce(const ReductionContext& ReductionContext) override;
};

class ComparisonOperator : public Expression {
public:
  ComparisonOperator(const ComparisonOperatorNode * n) : Expression(n) {}
  static bool IsComparisonOperatorType(ExpressionNode::Type type);
  static ExpressionNode::Type Opposite(ExpressionNode::Type type);

  enum class TrinaryBoolean : uint8_t {
    False = 0,
    True = 1,
    Unknown = 2
  };
  static TrinaryBoolean TrinaryTruthValue(ExpressionNode::Type type, TrinaryBoolean chidlrenAreEqual, TrinaryBoolean leftChildIsGreater);

  // For the comparison A ? B, create the reduced expression A-B
  Expression standardEquation(Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, Preferences::UnitFormat unitFormat, ExpressionNode::ReductionTarget reductionTarget) const;

  Expression shallowReduce(const ExpressionNode::ReductionContext& reductionContext);
};

}

#endif
