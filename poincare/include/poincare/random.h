#ifndef POINCARE_RANDOM_H
#define POINCARE_RANDOM_H

#include <poincare/static_hierarchy.h>
#include <poincare/layout_engine.h>

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
  ExpressionLayout * createLayout(PrintFloat::Mode floatDisplayMode, int numberOfSignificantDigits) const override {
    return LayoutEngine::createPrefixLayout(this, floatDisplayMode, numberOfSignificantDigits, name());
  }
  int writeTextInBuffer(char * buffer, int bufferSize, int numberOfSignificantDigits = PrintFloat::k_numberOfStoredSignificantDigits) const override {
    return LayoutEngine::writePrefixExpressionTextInBuffer(this, buffer, bufferSize, numberOfSignificantDigits, name());
  }
  const char * name() const { return "random"; }
  /* Evaluation */
  Evaluation<float> * privateApproximate(SinglePrecision p, Context& context, AngleUnit angleUnit) const override {
    return templateApproximate<float>();
  }
  Evaluation<double> * privateApproximate(DoublePrecision p, Context& context, AngleUnit angleUnit) const override {
    return templateApproximate<double>();
  }
  template <typename T> Evaluation<T> * templateApproximate() const {
    return new Complex<T>(random<T>());
  }
};

}

#endif
