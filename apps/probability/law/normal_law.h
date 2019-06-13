#ifndef PROBABILITE_NORMAL_LAW_H
#define PROBABILITE_NORMAL_LAW_H

#include "two_parameter_law.h"

namespace Probability {

class NormalLaw final : public TwoParameterLaw {
public:
  NormalLaw() : TwoParameterLaw(0.0f, 1.0f) {}
  I18n::Message title() override { return I18n::Message::NormalLaw; }
  Type type() const override { return Type::Normal; }
  bool isContinuous() const override { return true; }
  float xMin() const override { return xExtremum(true); }
  float xMax() const override { return xExtremum(false); }
  float yMax() const override;
  I18n::Message parameterNameAtIndex(int index) override;
  I18n::Message parameterDefinitionAtIndex(int index) override;
  float evaluateAtAbscissa(float x) const override;
  bool authorizedValueAtIndex(float x, int index) const override;
  void setParameterAtIndex(float f, int index) override;
  double cumulativeDistributiveFunctionAtAbscissa(double x) const override;
  double cumulativeDistributiveInverseForProbability(double * probability) override;
private:
  constexpr static double k_maxRatioMuSigma = 1000000.0f;
  /* For the standard normal law, P(X < y) > 0.9999995 with y >= 4.892 so the
   * value displayed is 1. But this is dependent on the fact that we display
   * only 7 decimal values! */
  static_assert(Constant::LargeNumberOfSignificantDigits == 7, "k_maxProbability is ill-defined compared to LargeNumberOfSignificantDigits");
  constexpr static double k_boundStandardNormalDistribution = 4.892;
  double standardNormalCumulativeDistributiveFunctionAtAbscissa(double abscissa) const;
  double standardNormalCumulativeDistributiveInverseForProbability(double probability);
  float xExtremum(bool min) const;
};

}

#endif
