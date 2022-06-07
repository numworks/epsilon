#ifndef POINCARE_MATRIX_DIMENSION_H
#define POINCARE_MATRIX_DIMENSION_H

#include <poincare/expression.h>

namespace Poincare {

class DimensionNode final : public ExpressionNode {
public:

  // TreeNode
  size_t size() const override { return sizeof(DimensionNode); }
  int numberOfChildren() const override;
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "Dimension";
  }
#endif

  // Properties
  Type type() const override { return Type::Dimension; }

private:
  // Layout
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  // Simplification
  Expression shallowReduce(ReductionContext reductionContext) override;
  LayoutShape leftLayoutShape() const override { return LayoutShape::MoreLetters; };
  LayoutShape rightLayoutShape() const override { return LayoutShape::BoundaryPunctuation; }
  // Evaluation
  Evaluation<float> approximate(SinglePrecision p, ApproximationContext approximationContext) const override { return templatedApproximate<float>(approximationContext); }
  Evaluation<double> approximate(DoublePrecision p, ApproximationContext approximationContext) const override { return templatedApproximate<double>(approximationContext); }
 template<typename T> Evaluation<T> templatedApproximate(ApproximationContext approximationContext) const;
};

class Dimension final : public Expression {
public:
  Dimension(const DimensionNode * n) : Expression(n) {}
  static Dimension Builder(Expression child) { return TreeHandle::FixedArityBuilder<Dimension, DimensionNode>({child}); }

  static constexpr Expression::FunctionHelper s_functionHelper = Expression::FunctionHelper("dim", 1, Initializer<DimensionNode>, sizeof(DimensionNode));

  Expression shallowReduce(Context * context);
};

}

#endif
