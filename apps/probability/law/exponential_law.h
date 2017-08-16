#ifndef PROBABILITE_EXPONENTIAL_LAW_H
#define PROBABILITE_EXPONENTIAL_LAW_H

#include "one_parameter_law.h"

namespace Probability {

class ExponentialLaw : public OneParameterLaw {
public:
  ExponentialLaw();
  I18n::Message title() override;
  Type type() const override;
  bool isContinuous() const override;
  float xMin() override;
  float yMin() override;
  float xMax() override;
  float yMax() override;
  I18n::Message parameterNameAtIndex(int index) override;
  I18n::Message parameterDefinitionAtIndex(int index) override;
  float evaluateAtAbscissa(float x) const override;
  bool authorizedValueAtIndex(float x, int index) const override;
  double cumulativeDistributiveFunctionAtAbscissa(double x) const override;
  double cumulativeDistributiveInverseForProbability(double * probability) override;
};

}

#endif
