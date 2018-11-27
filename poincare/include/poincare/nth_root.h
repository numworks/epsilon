#ifndef POINCARE_NTH_ROOT_H
#define POINCARE_NTH_ROOT_H

#include <poincare/expression.h>
#include <poincare/complex_helper.h>

namespace Poincare {

class NthRootNode final : public ExpressionNode  {
public:
  // ExpressionNode
  Type type() const override { return Type::NthRoot; }

  // TreeNode
  size_t size() const override { return sizeof(NthRootNode); }
  int numberOfChildren() const override;
#if POINCARE_TREE_LOG
  virtual void logNodeName(std::ostream & stream) const override {
    stream << "NthRoot";
  }
#endif

  // Complex
  Expression realPart(Context & context, Preferences::AngleUnit angleUnit) const override { return ComplexHelper::realPartFromPolarParts(this, context, angleUnit); }
  Expression imaginaryPart(Context & context, Preferences::AngleUnit angleUnit) const override { return ComplexHelper::imaginaryPartFromPolarParts(this, context, angleUnit); }
  Expression complexNorm(Context & context, Preferences::AngleUnit angleUnit) const override { return complexPolarPart(context, angleUnit, true); }
  Expression complexArgument(Context & context, Preferences::AngleUnit angleUnit) const override { return complexPolarPart(context, angleUnit, false); }

private:
  // Complex
  Expression complexPolarPart(Context & context, Preferences::AngleUnit angleUnit, bool isNorm) const;
  // Layout
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  // Simplification
  Expression shallowReduce(Context & context, Preferences::AngleUnit angleUnit, ReductionTarget target) override;
  // Evaluation
  Evaluation<float> approximate(SinglePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override { return templatedApproximate<float>(context, angleUnit); }
  Evaluation<double> approximate(DoublePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override { return templatedApproximate<double>(context, angleUnit); }
 template<typename T> Evaluation<T> templatedApproximate(Context& context, Preferences::AngleUnit angleUnit) const;

};

class NthRoot final : public Expression {
public:
  NthRoot(const NthRootNode * n) : Expression(n) {}
  static NthRoot Builder(Expression child0, Expression child1) { return NthRoot(child0, child1); }
  static Expression UntypedBuilder(Expression children) { return Builder(children.childAtIndex(0), children.childAtIndex(1)); }
  static constexpr Expression::FunctionHelper s_functionHelper = Expression::FunctionHelper("root", 2, &UntypedBuilder);

  Expression shallowReduce(Context & context, Preferences::AngleUnit angleUnit, ExpressionNode::ReductionTarget target);
private:
  NthRoot(Expression child0, Expression child1) : Expression(TreePool::sharedPool()->createTreeNode<NthRootNode>()) {
    replaceChildAtIndexInPlace(0, child0);
    replaceChildAtIndexInPlace(1, child1);
  }
};

}

#endif
