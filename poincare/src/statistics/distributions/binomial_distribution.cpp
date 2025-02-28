#include <assert.h>
#include <omg/float.h>
#include <poincare/src/solver/regularized_incomplete_beta_function.h>
#include <poincare/src/solver/solver_algorithms.h>
#include <poincare/src/statistics/distributions/binomial_distribution.h>

#include <cmath>

namespace Poincare::Internal::BinomialDistribution {

template <typename T>
T EvaluateAtAbscissa(T x, const T* parameters) {
  const T n = parameters[0];
  const T p = parameters[1];
  constexpr T precision = OMG::Float::Epsilon<T>();
  bool nIsZero = std::abs(n) < precision;
  bool pIsZero = std::abs(p) < precision;
  bool pIsOne = !pIsZero && std::abs(p - static_cast<T>(1.0)) < precision;
  if (nIsZero) {
    if (pIsZero || pIsOne) {
      return NAN;
    }
    if (std::floor(x) == 0) {
      return static_cast<T>(1.0);
    }
    return static_cast<T>(0.0);
  }
  if (pIsOne) {
    return static_cast<T>(floor(x) == n ? 1.0 : 0.0);
  }
  if (pIsZero) {
    return static_cast<T>(floor(x) == 0 ? 1.0 : 0.0);
  }
  if (x > n) {
    return static_cast<T>(0.0);
  }
  T lResult = std::lgamma(n + static_cast<T>(1.0)) -
              std::lgamma(std::floor(x) + static_cast<T>(1.0)) -
              std::lgamma(n - std::floor(x) + static_cast<T>(1.0)) +
              std::floor(x) * std::log(p) +
              (n - std::floor(x)) * std::log(static_cast<T>(1.0) - p);
  return std::exp(lResult);
}

template <typename T>
T CumulativeDistributiveFunctionAtAbscissa(T x, const T* parameters) {
  T n = parameters[k_nIndex];
  T p = parameters[k_pIndex];

  if (std::isinf(x)) {
    return x > static_cast<T>(0.0) ? static_cast<T>(1.0) : static_cast<T>(0.0);
  }
  if (x < static_cast<T>(0.0)) {
    return static_cast<T>(0.0);
  }
  if (x >= n) {
    return static_cast<T>(1.0);
  }
  T floorX = std::floor(x);
  return RegularizedIncompleteBetaFunction(n - floorX, floorX + 1.0, 1.0 - p);
}

template <typename T>
T CumulativeDistributiveInverseForProbability(T probability,
                                              const T* parameters) {
  const T n = parameters[0];
  const T p = parameters[1];
  constexpr T precision = OMG::Float::Epsilon<T>();
  bool nIsZero = std::abs(n) < precision;
  bool pIsZero = std::abs(p) < precision;
  bool pIsOne = !pIsZero && std::abs(p - static_cast<T>(1.0)) < precision;
  if (nIsZero && (pIsZero || pIsOne)) {
    return NAN;
  }
  if (std::abs(probability) < precision) {
    if (pIsOne) {
      return 0;
    }
    return NAN;
  }
  if (std::abs(probability - static_cast<T>(1.0)) < precision) {
    return n;
  }
  T proba = probability;
  return SolverAlgorithms::CumulativeDistributiveInverseForNDefinedFunction<T>(
      &proba,
      [](T x, const void* auxiliary) {
        const T* params = reinterpret_cast<const T*>(auxiliary);
        return BinomialDistribution::EvaluateAtAbscissa(x, params);
      },
      parameters);
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

}  // namespace Poincare::Internal::BinomialDistribution
