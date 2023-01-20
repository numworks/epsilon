#ifndef POINCARE_PARENTHESIS_H
#define POINCARE_PARENTHESIS_H

#include <poincare/expression.h>
#include <poincare/complex_cartesian.h>

namespace Poincare {

class ParenthesisNode final : public ExpressionNode {
public:

  // TreeNode
  size_t size() const override { return sizeof(ParenthesisNode); }
  int numberOfChildren() const override { return 1; }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "Parenthesis";
  }
#endif

  // Properties
  TrinaryBoolean isPositive(Context * context) const override { return childAtIndex(0)->isPositive(context); }
  TrinaryBoolean isNull(Context * context) const override { return childAtIndex(0)->isNull(context); }
  Type type() const override { return Type::Parenthesis; }
  int polynomialDegree(Context * context, const char * symbolName) const override;
  /* This implementation of getPolynomialCoefficients is only supposed to be
   * called if a reduction failure occured. If other unreduced nodes prove
   * problematic, we might want to signal the reduction failure to the caller
   * so that it does not attempt to analyze the unreduced expression. */
  int getPolynomialCoefficients(Context * context, const char * symbolName, Expression coefficients[]) const override { return childAtIndex(0)->getPolynomialCoefficients(context, symbolName, coefficients); }
  Expression removeUnit(Expression * unit) override { assert(false); return ExpressionNode::removeUnit(unit); }

  // Layout
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits, Context * context) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  // Simplification
  Expression shallowReduce(const ReductionContext& reductionContext) override;
  LayoutShape leftLayoutShape() const override { return LayoutShape::BoundaryPunctuation; };

  // Approximation
  Evaluation<float> approximate(SinglePrecision p, const ApproximationContext& approximationContext) const override { return templatedApproximate<float>(approximationContext); }
  Evaluation<double> approximate(DoublePrecision p, const ApproximationContext& approximationContext) const override { return templatedApproximate<double>(approximationContext); }
private:
 template<typename T> Evaluation<T> templatedApproximate(const ApproximationContext& approximationContext) const;
};

class Parenthesis final : public ExpressionOneChild<Parenthesis, ParenthesisNode> {
public:
  using ExpressionBuilder::ExpressionBuilder;
  // Expression
  Expression shallowReduce(ReductionContext reductionContext);
};

}

#endif
