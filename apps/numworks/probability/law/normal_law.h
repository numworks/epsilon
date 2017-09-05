#ifndef PROBABILITE_NORMAL_LAW_H
#define PROBABILITE_NORMAL_LAW_H

#include "two_parameter_law.h"

namespace Probability {

class NormalLaw : public TwoParameterLaw {
public:
  NormalLaw();
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
  constexpr static double k_maxRatioMuSigma = 1000.0f;
  /* Waissi & Rossin constants */
  constexpr static double k_beta1 = -0.0004406;
  constexpr static double k_beta2 = 0.0418198;
  constexpr static double k_beta3 = 0.9;
  constexpr static double k_boundStandardNormalDistribution = 3.291;
  constexpr static double k_alpha1 = 22.0;
  constexpr static double k_alpha2 = 41.0;
  constexpr static double k_alpha3 = 10.0;
  double standardNormalCumulativeDistributiveFunctionAtAbscissa(double abscissa) const;
  double standardNormalCumulativeDistributiveInverseForProbability(double probability);
};

}

#endif
