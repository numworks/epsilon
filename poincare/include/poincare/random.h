#ifndef POINCARE_RANDOM_H
#define POINCARE_RANDOM_H

#include <poincare/static_hierarchy.h>
#include <poincare/layout_engine.h>
#include <poincare/complex.h>

namespace Poincare {

class Random : public StaticHierarchy<0> {
  using StaticHierarchy<0>::StaticHierarchy;
public:
  Type type() const override;
  Expression * clone() const override;
  Sign sign() const override { return Sign::Positive; }
  template<typename T> static T random();
private:
  Expression * setSign(Sign s, Context & context, AngleUnit angleUnit) override;
  /* Layout */
  ExpressionLayout * privateCreateLayout(PrintFloat::Mode floatDisplayMode, ComplexFormat complexFormat) const override {
    return LayoutEngine::createPrefixLayout(this, floatDisplayMode, complexFormat, name());
  }
  int writeTextInBuffer(char * buffer, int bufferSize, int numberOfSignificantDigits = PrintFloat::k_numberOfStoredSignificantDigits) const override {
    return LayoutEngine::writePrefixExpressionTextInBuffer(this, buffer, bufferSize, numberOfSignificantDigits, name());
  }
  const char * name() const { return "random"; }
  /* Evaluation */
  Expression * privateApproximate(SinglePrecision p, Context& context, AngleUnit angleUnit) const override {
    return templateApproximate<float>();
  }
  Expression * privateApproximate(DoublePrecision p, Context& context, AngleUnit angleUnit) const override {
    return templateApproximate<double>();
  }
  template <typename T> Expression * templateApproximate() const {
    return new Complex<T>(Complex<T>::Float(random<T>()));
  }
};

}

#endif
