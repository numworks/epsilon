#ifndef PROBABILITE_BINOMIAL_LAW_H
#define PROBABILITE_BINOMIAL_LAW_H

#include "two_parameter_law.h"

namespace Probability {

class BinomialLaw final : public TwoParameterLaw {
public:
  BinomialLaw() : TwoParameterLaw(20.0, 0.5) {}
  I18n::Message title() override { return I18n::Message::BinomialLaw; }
  Type type() const override { return Type::Binomial; }
  bool isContinuous() const override { return false; }
  float xMin() const override;
  float xMax() const override;
  float yMax() const override;
  I18n::Message parameterNameAtIndex(int index) override;
  I18n::Message parameterDefinitionAtIndex(int index) override;
  float evaluateAtAbscissa(float x) const override {
    return templatedApproximateAtAbscissa(x);
  }
  bool authorizedValueAtIndex(float x, int index) const override;
  double cumulativeDistributiveInverseForProbability(double * probability) override;
  double rightIntegralInverseForProbability(double * probability) override;
protected:
  double evaluateAtDiscreteAbscissa(int k) const override {
    return templatedApproximateAtAbscissa((double)k);
  }
  template<typename T> T templatedApproximateAtAbscissa(T x) const;
};

}

#endif
