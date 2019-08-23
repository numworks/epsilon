#ifndef POINCARE_NORMAL_DISTRIBUTION_H
#define POINCARE_NORMAL_DISTRIBUTION_H

#include <poincare/preferences.h>

namespace Poincare {

class NormalDistribution final {
public:
  template<typename T> static T EvaluateAtAbscissa(T x, T mu, T var);
  template<typename T> static T CumulativeDistributiveFunctionAtAbscissa(T x, T mu, T var);
  template<typename T> static T CumulativeDistributiveInverseForProbability(T probability, T mu, T var);
private:
  /* For the standard normal distribution, P(X < y) > 0.9999995 for y >= 4.892 so the
   * value displayed is 1. But this is dependent on the fact that we display
   * only 7 decimal values! */
  static_assert(Preferences::LargeNumberOfSignificantDigits == 7, "k_boundStandardNormalDistribution is ill-defined compared to LargeNumberOfSignificantDigits");
  constexpr static double k_boundStandardNormalDistribution = 4.892;
  template<typename T> static T StandardNormalCumulativeDistributiveFunctionAtAbscissa(T abscissa);
  template<typename T> static T StandardNormalCumulativeDistributiveInverseForProbability(T probability);
};

}

#endif
