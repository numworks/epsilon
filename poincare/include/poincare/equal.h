#ifndef POINCARE_EQUAL_H
#define POINCARE_EQUAL_H

#include <poincare/expression.h>

namespace Poincare {

class EqualNode final : public ExpressionNode {
public:

  // TreeNode
  size_t size() const override { return sizeof(EqualNode); }
  int numberOfChildren() const override { return 2; }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "Equal";
  }
#endif

  // ExpressionNode
  Type type() const override { return Type::Equal; }
  int polynomialDegree(Context * context, const char * symbolName) const override { return -1; }
private:
  // Simplification
  Expression shallowReduce(ReductionContext reductionContext) override;
  LayoutShape leftLayoutShape() const override { assert(false); return LayoutShape::BoundaryPunctuation; };
  // Layout
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  // Evaluation
  Evaluation<float> approximate(SinglePrecision p, ApproximationContext approximationContext) const override { return templatedApproximate<float>(approximationContext); }
  Evaluation<double> approximate(DoublePrecision p, ApproximationContext approximationContext) const override { return templatedApproximate<double>(approximationContext); }
  template<typename T> Evaluation<T> templatedApproximate(ApproximationContext approximationContext) const;
};

class Equal final : public Expression {
public:
  Equal(const EqualNode * n) : Expression(n) {}
  static Equal Builder(Expression child0, Expression child1) { return TreeHandle::FixedArityBuilder<Equal, EqualNode>({child0, child1}); }

  // For the equation A = B, create the reduced expression A-B
  Expression standardEquation(Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, Preferences::UnitFormat unitFormat, ExpressionNode::ReductionTarget reductionTarget) const;
  // Expression
  Expression shallowReduce(ExpressionNode::ReductionContext reductionContext);
};

}

#endif
