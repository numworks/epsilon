#ifndef PROBABILITE_UNIFORM_LAW_H
#define PROBABILITE_UNIFORM_LAW_H

#include "two_parameter_law.h"

namespace Probability {

class UniformLaw : public TwoParameterLaw {
public:
  UniformLaw();
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
  void setParameterAtIndex(float f, int index) override;
  double cumulativeDistributiveFunctionAtAbscissa(double x) const override;
  double cumulativeDistributiveInverseForProbability(double * probability) override;
private:
  constexpr static float k_diracMaximum = 10.0f;
  constexpr static float k_diracWidth = 0.005f;
};

}

#endif
