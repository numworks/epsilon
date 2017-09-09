#ifndef PROBABILITE_POISSON_LAW_H
#define PROBABILITE_POISSON_LAW_H

#include "one_parameter_law.h"

namespace Probability {

class PoissonLaw : public OneParameterLaw {
public:
  PoissonLaw();
  const I18n::Message *title() override;
  Type type() const override;
  bool isContinuous() const override;
  float xMin() override;
  float yMin() override;
  float xMax() override;
  float yMax() override;
  const I18n::Message *parameterNameAtIndex(int index) override;
  const I18n::Message *parameterDefinitionAtIndex(int index) override;
  float evaluateAtAbscissa(float x) const override {
    return templatedEvaluateAtAbscissa(x);
  }
  bool authorizedValueAtIndex(float x, int index) const override;
private:
  double evaluateAtDiscreteAbscissa(int k) const override {
    return templatedEvaluateAtAbscissa((double)k);
  }
  template<typename T> T templatedEvaluateAtAbscissa(T x) const;
};

}

#endif
