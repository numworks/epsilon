#ifndef POINCARE_COSINE_H
#define POINCARE_COSINE_H

#include <poincare/layout_engine.h>
#include <poincare/static_hierarchy.h>
#include <poincare/approximation_engine.h>
#include <poincare/trigonometry.h>
#include <poincare/expression.h>

namespace Poincare {

class Cosine : public StaticHierarchy<1>::StaticHierarchy  {
  using StaticHierarchy<1>::StaticHierarchy;
  friend class Tangent;
public:
  Type type() const override;
  Expression * clone() const override;
  float characteristicXRange(Context & context, AngleUnit angleUnit) const override;
  template<typename T> static std::complex<T> computeOnComplex(const std::complex<T> c, AngleUnit angleUnit = AngleUnit::Radian) {
    return Trigonometry::computeDirectOnComplex(c, angleUnit, std::cos);
  }
private:
  /* Layout */
  ExpressionLayout * createLayout(PrintFloat::Mode floatDisplayMode, int numberOfSignificantDigits) const override {
    return LayoutEngine::createPrefixLayout(this, floatDisplayMode, numberOfSignificantDigits, name());
  }
  int writeTextInBuffer(char * buffer, int bufferSize, PrintFloat::Mode floatDisplayMode, int numberOfSignificantDigits) const override {
    return LayoutEngine::writePrefixExpressionTextInBuffer(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, name());
  }
  const char * name() const { return "cos"; }
  /* Simplication */
  Expression * shallowReduce(Context& context, AngleUnit angleUnit) override;
  /* Evaluation */
  Evaluation<float> * privateApproximate(SinglePrecision p, Context& context, AngleUnit angleUnit) const override {
    return ApproximationEngine::map<float>(this, context, angleUnit,computeOnComplex<float>);
  }
  Evaluation<double> * privateApproximate(DoublePrecision p, Context& context, AngleUnit angleUnit) const override {
    return ApproximationEngine::map<double>(this, context, angleUnit, computeOnComplex<double>);
  }
};

}

#endif
