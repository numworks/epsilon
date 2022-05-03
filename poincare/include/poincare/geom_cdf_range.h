#ifndef POINCARE_GEOM_CDF_RANGE_H
#define POINCARE_GEOM_CDF_RANGE_H

#include <poincare/approximation_helper.h>
#include <poincare/geometric_distribution_function.h>

namespace Poincare {

class GeomCDFRangeNode final : public GeometricDistributionFunctionNode  {
public:

  // TreeNode
  size_t size() const override { return sizeof(GeomCDFRangeNode); }
  int numberOfChildren() const override;
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "GeomCDFRange";
  }
#endif

  // Properties
  Type type() const override { return Type::GeomCDFRange; }
  Sign sign(Context * context) const override { return Sign::Positive; }

private:
  // Layout
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;

  // Evaluation
  Evaluation<float> approximate(SinglePrecision p, ApproximationContext approximationContext) const override { return templatedApproximate<float>(approximationContext); }
  Evaluation<double> approximate(DoublePrecision p, ApproximationContext approximationContext) const override { return templatedApproximate<double>(approximationContext); }
  template<typename T> Evaluation<T> templatedApproximate(ApproximationContext approximationContext) const;
};

class GeomCDFRange final : public GeometricDistributionFunction {
public:
  GeomCDFRange(const GeomCDFRangeNode * n) : GeometricDistributionFunction(n) {}
  static GeomCDFRange Builder(Expression child0, Expression child1, Expression child2) { return TreeHandle::FixedArityBuilder<GeomCDFRange, GeomCDFRangeNode>({child0, child1, child2}); }
  static constexpr Expression::FunctionHelper s_functionHelper = Expression::FunctionHelper("geomcdfrange", 3, &UntypedBuilderThreeChildren<GeomCDFRange>);
};

}

#endif
