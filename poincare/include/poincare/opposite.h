#ifndef POINCARE_OPPOSITE_H
#define POINCARE_OPPOSITE_H

#include <poincare/expression.h>
#include <poincare/approximation_helper.h>

namespace Poincare {

class Opposite;

class OppositeNode /*final*/ : public ExpressionNode {
public:
  template<typename T> static Complex<T> compute(const std::complex<T> c, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit = Preferences::AngleUnit::Degree) { return Complex<T>::Builder(-c); }


  // TreeNode
  size_t size() const override { return sizeof(OppositeNode); }
  int numberOfChildren() const override { return 1; }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "Opposite";
  }
#endif

  // Properties
  Type type() const override { return Type::Opposite; }
  int polynomialDegree(Context * context, const char * symbolName) const override;
  Sign sign(Context * context) const override;
  bool childAtIndexNeedsUserParentheses(const Expression & child, int childIndex) const override;

  // Approximation
  Evaluation<float> approximate(SinglePrecision p, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const override {
    return ApproximationHelper::Map<float>(this, context, complexFormat, angleUnit, compute<float>);
  }
  Evaluation<double> approximate(DoublePrecision p, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const override {
    return ApproximationHelper::Map<double>(this, context, complexFormat, angleUnit, compute<double>);
  }

  // Layout
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode = Preferences::PrintFloatMode::Decimal, int numberOfSignificantDigits = 0) const override;

  // Simplification
  Expression shallowReduce(ReductionContext reductionContext) override;
  LayoutShape leftLayoutShape() const override { assert(false); return LayoutShape::OneLetter; };
  LayoutShape rightLayoutShape() const override { return childAtIndex(0)->rightLayoutShape(); }
};

class Opposite final : public Expression {
public:
  Opposite(const OppositeNode * n) : Expression(n) {}
  static Opposite Builder() { return TreeHandle::FixedArityBuilder<Opposite, OppositeNode>(); }
  static Opposite Builder(Expression child) { return TreeHandle::FixedArityBuilder<Opposite, OppositeNode>({child}); }

  Expression shallowReduce(ExpressionNode::ReductionContext reductionContext);
};

}

#endif
