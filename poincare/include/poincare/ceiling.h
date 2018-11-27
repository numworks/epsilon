#ifndef POINCARE_CEILING_H
#define POINCARE_CEILING_H

#include <poincare/approximation_helper.h>
#include <poincare/expression.h>
#include <poincare/complex_helper.h>

namespace Poincare {

class CeilingNode final : public ExpressionNode  {
public:

  // TreeNode
  size_t size() const override { return sizeof(CeilingNode); }
  int numberOfChildren() const override;
#if POINCARE_TREE_LOG
  virtual void logNodeName(std::ostream & stream) const override {
    stream << "Ceiling";
  }
#endif

  // Complex
  Expression realPart(Context & context, Preferences::AngleUnit angleUnit) const override { return ComplexHelper::realPartRealFunction(this, context, angleUnit); }
  Expression imaginaryPart(Context & context, Preferences::AngleUnit angleUnit) const override { return ComplexHelper::imaginaryPartRealFunction(this, context, angleUnit); }

  // Properties
  Type type() const override { return Type::Ceiling; }
private:
  // Layout
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  // Simplification
  Expression shallowReduce(Context & context, Preferences::AngleUnit angleUnit, ReductionTarget target) override;
  // Evaluation
  template<typename T> static Complex<T> computeOnComplex(const std::complex<T> c, Preferences::AngleUnit angleUnit);
  Evaluation<float> approximate(SinglePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override {
    return ApproximationHelper::Map<float>(this, context, angleUnit, computeOnComplex<float>);
  }
  Evaluation<double> approximate(DoublePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override {
    return ApproximationHelper::Map<double>(this, context, angleUnit, computeOnComplex<double>);
  }
};

class Ceiling final : public Expression {
public:
  Ceiling(const CeilingNode * n) : Expression(n) {}
  static Ceiling Builder(Expression child) { return Ceiling(child); }
  static Expression UntypedBuilder(Expression children) { return Builder(children.childAtIndex(0)); }
  static constexpr Expression::FunctionHelper s_functionHelper = Expression::FunctionHelper("ceil", 1, &UntypedBuilder);

  Expression shallowReduce(Context & context, Preferences::AngleUnit angleUnit);
private:
  explicit Ceiling(Expression child) : Expression(TreePool::sharedPool()->createTreeNode<CeilingNode>()) {
    replaceChildAtIndexInPlace(0, child);
  }
};

}

#endif


