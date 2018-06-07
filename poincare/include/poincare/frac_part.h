#ifndef POINCARE_FRAC_PART_H
#define POINCARE_FRAC_PART_H

#include <poincare/layout_engine.h>
#include <poincare/static_hierarchy.h>
#include <poincare/approximation_engine.h>

namespace Poincare {

class FracPart : public StaticHierarchy<1>  {
  using StaticHierarchy<1>::StaticHierarchy;
public:
  Type type() const override;
  Expression * clone() const override;
private:
  /* Layout */
  ExpressionLayout * createLayout(PrintFloat::Mode floatDisplayMode, int numberOfSignificantDigits) const override {
    return LayoutEngine::createPrefixLayout(this, floatDisplayMode, numberOfSignificantDigits, name());
  }
  int writeTextInBuffer(char * buffer, int bufferSize, PrintFloat::Mode floatDisplayMode, int numberOfSignificantDigits) const override {
    return LayoutEngine::writePrefixExpressionTextInBuffer(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, name());
  }
  const char * name() const { return "frac"; }
  /* Simplification */
  Expression * shallowReduce(Context& context, AngleUnit angleUnit) override;
  /* Evaluation */
  template<typename T> static std::complex<T> computeOnComplex(const std::complex<T> c, AngleUnit angleUnit);
  Evaluation<float> * privateApproximate(SinglePrecision p, Context& context, AngleUnit angleUnit) const override {
    return ApproximationEngine::map<float>(this, context, angleUnit, computeOnComplex<float>);
  }
  Evaluation<double> * privateApproximate(DoublePrecision p, Context& context, AngleUnit angleUnit) const override {
    return ApproximationEngine::map<double>(this, context, angleUnit, computeOnComplex<double>);
  }
};

}

#endif


