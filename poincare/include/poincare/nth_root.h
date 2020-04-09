#ifndef POINCARE_NTH_ROOT_H
#define POINCARE_NTH_ROOT_H

#include <poincare/expression.h>

namespace Poincare {

class NthRootNode final : public ExpressionNode  {
public:
  // ExpressionNode
  Type type() const override { return Type::NthRoot; }

  // TreeNode
  size_t size() const override { return sizeof(NthRootNode); }
  int numberOfChildren() const override;
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "NthRoot";
  }
#endif

private:
  Expression getUnit() const override { assert(false); return ExpressionNode::getUnit(); }
  // Layout
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  // Simplification
  Expression shallowReduce(ReductionContext reductionContext) override;
  LayoutShape leftLayoutShape() const override { return LayoutShape::NthRoot; };
  LayoutShape rightLayoutShape() const override { return LayoutShape::Root; };
  // Evaluation
  Evaluation<float> approximate(SinglePrecision p, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const override { return templatedApproximate<float>(context, complexFormat, angleUnit); }
  Evaluation<double> approximate(DoublePrecision p, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const override { return templatedApproximate<double>(context, complexFormat, angleUnit); }
 template<typename T> Evaluation<T> templatedApproximate(Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const;

};

class NthRoot final : public Expression {
public:
  NthRoot(const NthRootNode * n) : Expression(n) {}
  static NthRoot Builder(Expression child0, Expression child1) { return TreeHandle::FixedArityBuilder<NthRoot, NthRootNode>({child0, child1}); }
  static constexpr Expression::FunctionHelper s_functionHelper = Expression::FunctionHelper("root", 2, &UntypedBuilderTwoChildren<NthRoot>);

  Expression shallowReduce(ExpressionNode::ReductionContext reductionContext);
};

}

#endif
