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
  /* For the standard norma law, P(X < y) > 0.9999995  with y >= 4.892 so the
   * value displayed is 1. But this is dependent on the fact that we display
   * only 7 decimal values! */
  static_assert(Constant::LargeNumberOfSignificantDigits == 7, "k_maxProbability is ill-defined compared to LargeNumberOfSignificantDigits");
  constexpr static double k_boundStandardNormalDistribution = 4.892;
  double standardNormalCumulativeDistributiveFunctionAtAbscissa(double abscissa) const;
  double standardNormalCumulativeDistributiveInverseForProbability(double probability);
};

}

#endif
