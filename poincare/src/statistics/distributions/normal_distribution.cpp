#include <assert.h>
#include <omg/float.h>
#include <poincare/src/solver/erf_inv.h>
#include <poincare/src/statistics/distributions/normal_distribution.h>
#include <poincare/statistics/distribution.h>

#include <cmath>

#define M_SQRT_2PI 2.506628274631000502415765284811

namespace Poincare::Internal::NormalDistribution {

template <typename T>
T EvaluateAtAbscissa(T x, const Distribution::ParametersArray<T> params) {
  const T mu = params[k_muIndex];
  const T sigma = params[k_sigmaIndex];
  const float xMinusMuOverVar = (x - mu) / sigma;
  return (static_cast<T>(1.0)) /
         (std::fabs(sigma) * static_cast<T>(M_SQRT_2PI)) *
         std::exp(-(static_cast<T>(0.5)) * xMinusMuOverVar * xMinusMuOverVar);
}

template <typename T>
static T standardNormalCumulativeDistributiveFunctionAtAbscissa(T abscissa) {
  if (std::isnan(abscissa)) {
    return NAN;
  }
  constexpr static T k_standardMu =
      Distribution::DefaultParameterAtIndex(Distribution::Type::Normal, 0);
  if (std::isinf(abscissa)) {
    return abscissa > k_standardMu ? static_cast<T>(1.0) : static_cast<T>(0.0);
  }
  if (abscissa == k_standardMu) {
    return static_cast<T>(0.5);
  }
  if (abscissa < k_standardMu) {
    return (static_cast<T>(1.0)) -
           standardNormalCumulativeDistributiveFunctionAtAbscissa(-abscissa);
  }
  return (static_cast<T>(0.5)) +
         (static_cast<T>(0.5)) * std::erf(abscissa / static_cast<T>(M_SQRT2));
}

template <typename T>
static T standardNormalCumulativeDistributiveInverseForProbability(
    T probability) {
  if (probability > static_cast<T>(1.0) || probability < static_cast<T>(0.0) ||
      !std::isfinite(probability)) {
    return NAN;
  }
  constexpr T precision = OMG::Float::Epsilon<T>();
  if ((static_cast<T>(1.0)) - probability < precision) {
    return INFINITY;
  }
  if (probability < precision) {
    return -INFINITY;
  }
  if (probability < static_cast<T>(0.5)) {
    return -standardNormalCumulativeDistributiveInverseForProbability(
        (static_cast<T>(1.0)) - probability);
  }
  return static_cast<T>(M_SQRT2) *
         erfInv((static_cast<T>(2.0)) * probability - static_cast<T>(1.0));
}

template <typename T>
T CumulativeDistributiveFunctionAtAbscissa(
    T x, const Distribution::ParametersArray<T> params) {
  const T mu = params[k_muIndex];
  const T sigma = params[k_sigmaIndex];
  return standardNormalCumulativeDistributiveFunctionAtAbscissa<T>(
      (x - mu) / std::fabs(sigma));
}

template <typename T>
T CumulativeDistributiveInverseForProbability(
    T probability, const Distribution::ParametersArray<T> params) {
  const T mu = params[k_muIndex];
  const T sigma = params[k_sigmaIndex];
  return standardNormalCumulativeDistributiveInverseForProbability(
             probability) *
             std::fabs(sigma) +
         mu;
}

double EvaluateParameterForProbabilityAndBound(
    int parameterIndex, const Distribution::ParametersArray<double> parameters,
    double probability, double bound, bool isUpperBound) {
  if (std::isnan(probability)) {
    return NAN;
  }
  assert(probability >= 0.0 && probability <= 1.0);
  if (!isUpperBound) {
    probability = 1.0 - probability;
  }
  /* If X following (mu, sigma) is inferior to bound, then Z following (0, 1)
   * is inferior to (bound - mu)/sigma. */
  double abscissaForStandardDistribution =
      standardNormalCumulativeDistributiveInverseForProbability(probability);
  if (parameterIndex == 0) {  // mu
    return bound - parameters[1] * abscissaForStandardDistribution;
  }
  assert(parameterIndex == 1);  // sigma
  if (abscissaForStandardDistribution == 0) {
    if (bound == parameters[0]) {
      // Return default value if there is an infinity of possible sigma
      return Distribution::DefaultParameterAtIndex(Distribution::Type::Normal,
                                                   1);
    }
    return NAN;
  }
  double result = (bound - parameters[0]) / abscissaForStandardDistribution;
  return result > 0.0 ? result : NAN;  // Sigma can't be negative or null
}

template float EvaluateAtAbscissa<float>(
    float, const Distribution::ParametersArray<float>);
template double EvaluateAtAbscissa<double>(
    double, const Distribution::ParametersArray<double>);
template float CumulativeDistributiveFunctionAtAbscissa<float>(
    float, const Distribution::ParametersArray<float>);
template double CumulativeDistributiveFunctionAtAbscissa<double>(
    double, const Distribution::ParametersArray<double>);
template float CumulativeDistributiveInverseForProbability<float>(
    float, const Distribution::ParametersArray<float>);
template double CumulativeDistributiveInverseForProbability<double>(
    double, const Distribution::ParametersArray<double>);

}  // namespace Poincare::Internal::NormalDistribution
