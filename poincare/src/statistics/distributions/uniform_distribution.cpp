#include <assert.h>
#include <omg/float.h>
#include <poincare/src/solver/beta_function.h>
#include <poincare/src/solver/regularized_incomplete_beta_function.h>
#include <poincare/src/statistics/distributions/uniform_distribution.h>

namespace Poincare::Internal::UniformDistribution {

template <typename T>
T EvaluateAtAbscissa(T x, const Distribution::ParametersArray<T> params) {
  const T d1 = params[0];
  const T d2 = params[1];
  if (d1 <= x && x <= d2) {
    return (1.0 / (d2 - d1));
  }
  return 0.0;
}

template <typename T>
T CumulativeDistributiveFunctionAtAbscissa(
    T x, const Distribution::ParametersArray<T> params) {
  const T d1 = params[0];
  const T d2 = params[1];
  if (x <= d1) {
    return 0.0;
  }
  if (x < d2) {
    return (x - d1) / (d2 - d1);
  }
  return 1.0;
}

template <typename T>
T CumulativeDistributiveInverseForProbability(
    T probability, const Distribution::ParametersArray<T> params) {
  const T d1 = params[0];
  const T d2 = params[1];
  if (probability >= 1.0f) {
    return d2;
  }
  if (probability <= 0.0f) {
    return d1;
  }
  return d1 * (1 - probability) + probability * d2;
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

}  // namespace Poincare::Internal::UniformDistribution
