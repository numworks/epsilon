#ifndef POINCARE_GEOM_CDF_H
#define POINCARE_GEOM_CDF_H

#include <poincare/approximation_helper.h>
#include <poincare/geometric_distribution_function.h>

namespace Poincare {

class GeomCDFNode final : public GeometricDistributionFunctionNode  {
public:

  // TreeNode
  size_t size() const override { return sizeof(GeomCDFNode); }
  int numberOfChildren() const override;
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "GeomCDF";
  }
#endif

  // Properties
  Type type() const override { return Type::GeomCDF; }
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

class GeomCDF final : public GeometricDistributionFunction {
public:
  GeomCDF(const GeomCDFNode * n) : GeometricDistributionFunction(n) {}
  static GeomCDF Builder(Expression child0, Expression child1) { return TreeHandle::FixedArityBuilder<GeomCDF, GeomCDFNode>({child0, child1}); }
  static constexpr Expression::FunctionHelper s_functionHelper = Expression::FunctionHelper("geomcdf", 2, &UntypedBuilderTwoChildren<GeomCDF>);
};

}

#endif
