#ifndef POINCARE_SQUARE_ROOT_H
#define POINCARE_SQUARE_ROOT_H

#include <poincare/approximation_helper.h>
#include <poincare/expression.h>
#include <poincare/multiplication.h>
#include <ion/charset.h>

namespace Poincare {

class SquareRootNode /*final*/ : public ExpressionNode  {
public:
  // ExpressionNode
  Type type() const override { return Type::SquareRoot; }

  // TreeNode
  size_t size() const override { return sizeof(SquareRootNode); }
  int numberOfChildren() const override;
#if POINCARE_TREE_LOG
  virtual void logNodeName(std::ostream & stream) const override {
    stream << "SquareRoot";
  }
#endif

  // Complex
  ComplexCartesian complexCartesian(Context & context, Preferences::AngleUnit angleUnit) const override;
  /* If we use the formula arg(sqrt(a)) = arg(a)/2, we are likely to end up
   * with half arcTangent. To avoid that, we compute the argument(sqrt(a))
   * from the real and imaginary part of sqrt(a).
   * TODO: What about norm(sqrt(a))? */
  //ComplexPolar complexPolar(Context & context, Preferences::AngleUnit angleUnit) const override;

private:
  //Complex
  static Multiplication complexCartesianHelper(Expression e, Context & context, Preferences::AngleUnit angleUnit);
  // Layout
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  // Simplification
  Expression shallowReduce(Context & context, Preferences::AngleUnit angleUnit, ReductionTarget target) override;
  // Evaluation
  template<typename T> static Complex<T> computeOnComplex(const std::complex<T> c, Preferences::AngleUnit angleUnit);
  Evaluation<float> approximate(SinglePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override {
    return ApproximationHelper::Map<float>(this, context, angleUnit,computeOnComplex<float>);
  }
  Evaluation<double> approximate(DoublePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override {
    return ApproximationHelper::Map<double>(this, context, angleUnit, computeOnComplex<double>);
  }
};

class SquareRoot final : public Expression {
public:
  SquareRoot(const SquareRootNode * n) : Expression(n) {}
  static SquareRoot Builder(Expression child) { return SquareRoot(child); }
  static Expression UntypedBuilder(Expression children) { return Builder(children.childAtIndex(0)); }
  static_assert('\x91' == Ion::Charset::Root, "Charset error");
  static constexpr Expression::FunctionHelper s_functionHelper = Expression::FunctionHelper("\x91", 1, &UntypedBuilder);

  Expression shallowReduce(Context & context, Preferences::AngleUnit angleUnit, ExpressionNode::ReductionTarget target);
private:
  explicit SquareRoot(Expression child) : Expression(TreePool::sharedPool()->createTreeNode<SquareRootNode>()) {
    replaceChildAtIndexInPlace(0, child);
  }
  static const char k_name[2];
};

}

#endif
