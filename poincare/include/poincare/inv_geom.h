#ifndef POINCARE_INV_GEOM_H
#define POINCARE_INV_GEOM_H

#include <poincare/approximation_helper.h>
#include <poincare/geometric_distribution_function.h>

namespace Poincare {

class InvGeomNode final : public GeometricDistributionFunctionNode  {
public:

  // TreeNode
  size_t size() const override { return sizeof(InvGeomNode); }
  int numberOfChildren() const override;
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "InvGeom";
  }
#endif

  // Properties
  Type type() const override { return Type::InvGeom; }
  Sign sign(Context * context) const override { return Sign::Positive; }

private:
  // Layout
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;

  // Simplication
  Expression shallowReduce(ReductionContext reductionContext) override;

  // Evaluation
  Evaluation<float> approximate(SinglePrecision p, ApproximationContext approximationContext) const override { return templatedApproximate<float>(approximationContext); }
  Evaluation<double> approximate(DoublePrecision p, ApproximationContext approximationContext) const override { return templatedApproximate<double>(approximationContext); }
  template<typename T> Evaluation<T> templatedApproximate(ApproximationContext approximationContext) const;
};

class InvGeom final : public GeometricDistributionFunction {
public:
  InvGeom(const InvGeomNode * n) : GeometricDistributionFunction(n) {}
  static InvGeom Builder(Expression child0, Expression child1) { return TreeHandle::FixedArityBuilder<InvGeom, InvGeomNode>({child0, child1}); }
  static constexpr Expression::FunctionHelper s_functionHelper = Expression::FunctionHelper("invgeom", 2, &UntypedBuilderTwoChildren<InvGeom>);
  Expression shallowReduce(ExpressionNode::ReductionContext reductionContext);
};

}

#endif
