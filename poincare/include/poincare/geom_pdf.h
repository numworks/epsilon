#ifndef POINCARE_GEOM_PDF_H
#define POINCARE_GEOM_PDF_H

#include <poincare/approximation_helper.h>
#include <poincare/geometric_distribution_function.h>

namespace Poincare {

class GeomPDFNode final : public GeometricDistributionFunctionNode  {
public:

  // TreeNode
  size_t size() const override { return sizeof(GeomPDFNode); }
  int numberOfChildren() const override;
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "GeomPDF";
  }
#endif

  // Properties
  Type type() const override { return Type::GeomPDF; }
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

class GeomPDF final : public GeometricDistributionFunction {
public:
  GeomPDF(const GeomPDFNode * n) : GeometricDistributionFunction(n) {}
  static GeomPDF Builder(Expression child0, Expression child1) { return TreeHandle::FixedArityBuilder<GeomPDF, GeomPDFNode>({child0, child1}); }
  static constexpr Expression::FunctionHelper s_functionHelper = Expression::FunctionHelper("geompdf", 2, &UntypedBuilderTwoChildren<GeomPDF>);
};

}

#endif
