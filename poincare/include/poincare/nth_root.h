#ifndef POINCARE_NTH_ROOT_H
#define POINCARE_NTH_ROOT_H

#include <poincare/expression.h>

namespace Poincare {

class NthRootNode final : public ExpressionNode  {
public:
  static constexpr char k_functionName[] = "root";
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
  Expression removeUnit(Expression * unit) override { assert(false); return ExpressionNode::removeUnit(unit); }
  // Layout
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  // Simplification
  Expression shallowReduce(const ReductionContext& reductionContext) override;
  LayoutShape leftLayoutShape() const override { return LayoutShape::NthRoot; };
  LayoutShape rightLayoutShape() const override { return LayoutShape::Root; };
  // Evaluation
  Evaluation<float> approximate(SinglePrecision p, ApproximationContext approximationContext) const override { return templatedApproximate<float>(approximationContext); }
  Evaluation<double> approximate(DoublePrecision p, ApproximationContext approximationContext) const override { return templatedApproximate<double>(approximationContext); }
 template<typename T> Evaluation<T> templatedApproximate(ApproximationContext approximationContext) const;

};

class NthRoot final : public ExpressionTwoChildren<NthRoot, NthRootNode> {
public:
  using ExpressionBuilder::ExpressionBuilder;
  Expression shallowReduce(const ExpressionNode::ReductionContext& reductionContext);
};

}

#endif
