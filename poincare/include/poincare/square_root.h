#ifndef POINCARE_SQUARE_ROOT_H
#define POINCARE_SQUARE_ROOT_H

#include <poincare/approximation_helper.h>
#include <poincare/expression.h>
#include <poincare/multiplication.h>

namespace Poincare {

class SquareRootNode /*final*/ : public ExpressionNode  {
public:
  // ExpressionNode
  Type type() const override { return Type::SquareRoot; }

  // TreeNode
  size_t size() const override { return sizeof(SquareRootNode); }
  int numberOfChildren() const override;
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "SquareRoot";
  }
#endif

private:
  // Layout
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  // Simplification
  Expression shallowReduce(ReductionContext reductionContext) override;
  LayoutShape leftLayoutShape() const override { return LayoutShape::Root; };
  // Evaluation
  template<typename T> static Complex<T> computeOnComplex(const std::complex<T> c, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit);
  Evaluation<float> approximate(SinglePrecision p, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const override {
    return ApproximationHelper::Map<float>(this, context, complexFormat, angleUnit,computeOnComplex<float>);
  }
  Evaluation<double> approximate(DoublePrecision p, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const override {
    return ApproximationHelper::Map<double>(this, context, complexFormat, angleUnit, computeOnComplex<double>);
  }
};

class SquareRoot final : public Expression {
public:
  SquareRoot(const SquareRootNode * n) : Expression(n) {}
  static SquareRoot Builder(Expression child) { return TreeHandle::FixedArityBuilder<SquareRoot, SquareRootNode>({child}); }
  static constexpr Expression::FunctionHelper s_functionHelper = Expression::FunctionHelper("√", 1, &UntypedBuilderOneChild<SquareRoot>);
  Expression shallowReduce(ExpressionNode::ReductionContext reductionContext);
};

}

#endif
