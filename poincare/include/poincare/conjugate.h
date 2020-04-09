#ifndef POINCARE_CONJUGATE_H
#define POINCARE_CONJUGATE_H

#include <poincare/approximation_helper.h>
#include <poincare/expression.h>

namespace Poincare {

class ConjugateNode /*final*/ : public ExpressionNode  {
public:

  // TreeNode
  size_t size() const override { return sizeof(ConjugateNode); }
  int numberOfChildren() const override;
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "Conjugate";
  }
#endif

  // Properties
  Type type() const override { return Type::Conjugate; }
private:
  // Layout
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  // Simplification
  Expression shallowReduce(ReductionContext reductionContext) override;
  LayoutShape leftLayoutShape() const override { return childAtIndex(0)->leftLayoutShape(); };
  LayoutShape rightLayoutShape() const override { return childAtIndex(0)->rightLayoutShape(); }

  // Evaluation
  template<typename T> static Complex<T> computeOnComplex(const std::complex<T> c, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit);
  Evaluation<float> approximate(SinglePrecision p, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const override {
    return ApproximationHelper::Map<float>(this, context, complexFormat, angleUnit,computeOnComplex<float>);
  }
  Evaluation<double> approximate(DoublePrecision p, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const override {
    return ApproximationHelper::Map<double>(this, context, complexFormat, angleUnit, computeOnComplex<double>);
  }
};

class Conjugate final : public Expression {
public:
  Conjugate(const ConjugateNode * n) : Expression(n) {}
  static Conjugate Builder(Expression child) { return TreeHandle::FixedArityBuilder<Conjugate, ConjugateNode>({child}); }

  static constexpr Expression::FunctionHelper s_functionHelper = Expression::FunctionHelper("conj", 1, &UntypedBuilderOneChild<Conjugate>);;

  Expression shallowReduce(ExpressionNode::ReductionContext reductionContext);
};

}

#endif
