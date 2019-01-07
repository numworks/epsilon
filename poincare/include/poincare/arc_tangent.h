#ifndef POINCARE_ARC_TANGENT_H
#define POINCARE_ARC_TANGENT_H

#include <poincare/approximation_helper.h>
#include <poincare/expression.h>
#include <poincare/trigonometry.h>

namespace Poincare {

class ArcTangentNode final : public ExpressionNode {
public:

  // TreeNode
  size_t size() const override { return sizeof(ArcTangentNode); }
  int numberOfChildren() const override;
#if POINCARE_TREE_LOG
  virtual void logNodeName(std::ostream & stream) const override {
    stream << "ArcTangent";
  }
#endif

  // Properties
  Type type() const override { return Type::ArcTangent; }

  // Complex
  bool isReal(Context & context) const override { return childAtIndex(0)->isReal(context); }

private:
  // Layout
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  // Simplification
  Expression shallowReduce(Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ReductionTarget target) override;
  //Evaluation
  template<typename T> static Complex<T> computeOnComplex(const std::complex<T> c, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit);
  Evaluation<float> approximate(SinglePrecision p, Context& context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const override {
    return ApproximationHelper::Map<float>(this, context, complexFormat, angleUnit,computeOnComplex<float>);
  }
  Evaluation<double> approximate(DoublePrecision p, Context& context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const override {
    return ApproximationHelper::Map<double>(this, context, complexFormat, angleUnit, computeOnComplex<double>);
  }
};

class ArcTangent final : public Expression {
public:
  ArcTangent(const ArcTangentNode * n) : Expression(n) {}
  static ArcTangent Builder(Expression child) { return ArcTangent(child); }
  static Expression UntypedBuilder(Expression children) { return Builder(children.childAtIndex(0)); }
  static constexpr Expression::FunctionHelper s_functionHelper = Expression::FunctionHelper("atan", 1, &UntypedBuilder);

  Expression shallowReduce(Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ExpressionNode::ReductionTarget target);
private:
  explicit ArcTangent(Expression child) : Expression(TreePool::sharedPool()->createTreeNode<ArcTangentNode>()) {
    replaceChildAtIndexInPlace(0, child);
  }
};

}

#endif
