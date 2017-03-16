#ifndef PROBABILITE_NORMAL_LAW_H
#define PROBABILITE_NORMAL_LAW_H

#include "two_parameter_law.h"

namespace Probability {

class NormalLaw : public TwoParameterLaw {
public:
  NormalLaw();
  ~NormalLaw() override {};
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
  float cumulativeDistributiveFunctionAtAbscissa(float x) const override;
  float cumulativeDistributiveInverseForProbability(float * probability) override;
private:
  /* Waissi & Rossin constants */
  constexpr static float k_beta1 = -0.0004406f;
  constexpr static float k_beta2 = 0.0418198f;
  constexpr static float k_beta3 = 0.9f;
  constexpr static float k_boundStandardNormalDistribution = 3.291f;
  constexpr static float k_alpha1 = 22.0f;
  constexpr static float k_alpha2 = 41.0f;
  constexpr static float k_alpha3 = 10.0f;
  float standardNormalCumulativeDistributiveFunctionAtAbscissa(float abscissa) const;
  float standardNormalCumulativeDistributiveInverseForProbability(float probability);
};

}

#endif
