#ifndef POINCARE_FLOOR_H
#define POINCARE_FLOOR_H

#include <poincare/approximation_helper.h>
#include <poincare/expression.h>

namespace Poincare {

class FloorNode /*final*/ : public ExpressionNode {
public:

  // TreeNode
  size_t size() const override { return sizeof(FloorNode); }
  int numberOfChildren() const override;
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "Floor";
  }
#endif

  // Properties
  Type type() const override { return Type::Floor; }

private:
  // Layout
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  // Simplification
  Expression shallowReduce(ReductionContext reductionContext) override;
  LayoutShape leftLayoutShape() const override { return LayoutShape::BoundaryPunctuation; };

  // Evaluation
  template<typename T> static Complex<T> computeOnComplex(const std::complex<T> c, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit);
  Evaluation<float> approximate(SinglePrecision p, ApproximationContext approximationContext) const override {
    return ApproximationHelper::MapOneChild<float>(this, approximationContext, computeOnComplex<float>);
  }
  Evaluation<double> approximate(DoublePrecision p, ApproximationContext approximationContext) const override {
    return ApproximationHelper::MapOneChild<double>(this, approximationContext, computeOnComplex<double>);
  }
};

class Floor final : public Expression {
public:
  Floor(const FloorNode * n) : Expression(n) {}
  static Floor Builder(Expression child) { return TreeHandle::FixedArityBuilder<Floor, FloorNode>({child}); }

  static constexpr Expression::FunctionHelper s_functionHelper = Expression::FunctionHelper("floor", 1, Initializer<FloorNode>, sizeof(FloorNode));

  Expression shallowReduce(ExpressionNode::ReductionContext reductionContext);
};

}

#endif
