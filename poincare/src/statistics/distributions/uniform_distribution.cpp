#include <assert.h>
#include <omg/float.h>
#include <poincare/src/solver/beta_function.h>
#include <poincare/src/solver/regularized_incomplete_beta_function.h>
#include <poincare/src/statistics/distributions/uniform_distribution.h>

namespace Poincare::Internal::UniformDistribution {

template <typename T>
T EvaluateAtAbscissa(T x, const T* params) {
  const T d1 = params[0];
  const T d2 = params[1];
  if (d1 - d2 < OMG::Float::Epsilon<T>()) {
    if (d1 - k_diracWidth <= x && x <= d2 + k_diracWidth) {
      return 2.0 * k_diracMaximum;
    }
    return 0.0;
  }
  if (d1 <= x && x <= d2) {
    return (1.0 / (d2 - d1));
  }
  return 0.0;
}

template <typename T>
T CumulativeDistributiveFunctionAtAbscissa(T x, const T* params) {
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
T CumulativeDistributiveInverseForProbability(T probability, const T* params) {
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

template float EvaluateAtAbscissa<float>(float, const float*);
template double EvaluateAtAbscissa<double>(double, const double*);
template float CumulativeDistributiveFunctionAtAbscissa<float>(float,
                                                               const float*);
template double CumulativeDistributiveFunctionAtAbscissa<double>(double,
                                                                 const double*);
template float CumulativeDistributiveInverseForProbability<float>(float,
                                                                  const float*);
template double CumulativeDistributiveInverseForProbability<double>(
    double, const double*);

}  // namespace Poincare::Internal::UniformDistribution
