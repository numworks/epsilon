#ifndef POINCARE_NORMAL_DISTRIBUTION_H
#define POINCARE_NORMAL_DISTRIBUTION_H

#include <poincare/expression.h>
#include <poincare/preferences.h>

namespace Poincare {

class NormalDistribution final {
public:
  template<typename T> static T EvaluateAtAbscissa(T x, T mu, T sigma);
  template<typename T> static T CumulativeDistributiveFunctionAtAbscissa(T x, T mu, T sigma);
  template<typename T> static T CumulativeDistributiveInverseForProbability(T probability, T mu, T sigma);
  template<typename T> static bool MuAndSigmaAreOK(T mu, T sigma);
  /* ExpressionMuAndVarAreOK returns true if the expression could be verified.
   * The result of the verification is *result. */
  static bool ExpressionMuAndVarAreOK(bool * result, const Expression & mu, const Expression & sigma, Context * context);
private:
  /* For the standard normal distribution, P(X < y) > 0.99999995 for y >= 5.33 so the
   * value displayed is 1. But this is dependent on the fact that we display
   * only 7 decimal values! */
  static_assert(Preferences::LargeNumberOfSignificantDigits == 7, "k_boundStandardNormalDistribution is ill-defined compared to LargeNumberOfSignificantDigits");
  constexpr static double k_boundStandardNormalDistribution = 5.33;
  template<typename T> static T StandardNormalCumulativeDistributiveFunctionAtAbscissa(T abscissa);
  template<typename T> static T StandardNormalCumulativeDistributiveInverseForProbability(T probability);
};

}

#endif
