#ifndef POINCARE_SINE_H
#define POINCARE_SINE_H

#include <poincare/layout_engine.h>
#include <poincare/static_hierarchy.h>
#include <poincare/approximation_engine.h>
#include <poincare/trigonometry.h>

namespace Poincare {

class Sine : public StaticHierarchy<1> {
  using StaticHierarchy<1>::StaticHierarchy;
  friend class Tangent;
public:
  Type type() const override;
  Expression * clone() const override;
  template<typename T> static Complex<T> computeOnComplex(const Complex<T> c, AngleUnit angleUnit = AngleUnit::Radian);
private:
  /* Layout */
  ExpressionLayout * privateCreateLayout(PrintFloat::Mode floatDisplayMode, ComplexFormat complexFormat) const override {
    return LayoutEngine::createPrefixLayout(this, floatDisplayMode, complexFormat, name());
  }
  int writeTextInBuffer(char * buffer, int bufferSize, int numberOfSignificantDigits = PrintFloat::k_numberOfStoredSignificantDigits) const override {
    return LayoutEngine::writePrefixExpressionTextInBuffer(this, buffer, bufferSize, numberOfSignificantDigits, name());
  }
  const char * name() const { return "sin"; }
  /* Simplication */
  Expression * shallowReduce(Context& context, AngleUnit angleUnit) override;
  /* Evaluation */
  Expression * privateApproximate(SinglePrecision p, Context& context, AngleUnit angleUnit) const override {
    return ApproximationEngine::map<float>(this, context, angleUnit,computeOnComplex<float>);
  }
  Expression * privateApproximate(DoublePrecision p, Context& context, AngleUnit angleUnit) const override {
    return ApproximationEngine::map<double>(this, context, angleUnit, computeOnComplex<double>);
  }
};

}

#endif
