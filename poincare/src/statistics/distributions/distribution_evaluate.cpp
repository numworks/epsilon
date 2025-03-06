#include <assert.h>
#include <omg/float.h>
#include <omg/troolean.h>
#include <omg/unreachable.h>
#include <poincare/src/expression/approximation.h>
#include <poincare/src/solver/beta_function.h>
#include <poincare/src/solver/regularized_incomplete_beta_function.h>
#include <poincare/src/solver/solver_algorithms.h>
#include <poincare/statistics/distribution.h>

#define M_SQRT_2PI 2.506628274631000502415765284811

namespace Poincare::Internal::Distribution {

template <typename T>
T evalBinomial(T x, const Distribution::ParametersArray<T> parameters) {
  if (std::isinf(x)) {
    return NAN;
  }
  const T n = parameters[Params::Binomial::N];
  const T p = parameters[Params::Binomial::P];
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
T evalChi2(T x, const Distribution::ParametersArray<T> parameters) {
  if (x < 0.0) {
    return NAN;
  }
  if (x == 0.0) {
    return 0.0;
  }
  if (std::isinf(x)) {
    return 0.0;
  }
  const T k = parameters[0];
  const T halfk = k / 2.0;
  const T halfX = x / 2.0;
  return std::exp(-lgamma(halfk) - halfX + (halfk - 1.0) * std::log(halfX)) /
         2.0;
}

template <typename T>
T evalExponential(T x, const Distribution::ParametersArray<T> params) {
  if (x < static_cast<T>(0.0)) {
    return NAN;
  }
  const T lambda = params[0];
  return lambda * std::exp(-lambda * x);
}

template <typename T>
T evalFischer(T x, const Distribution::ParametersArray<T> params) {
  if (std::isinf(x)) {
    return NAN;
  }
  const T d1 = params[Params::Fisher::D1];
  const T d2 = params[Params::Fisher::D2];

  const T f = d1 * x / (d1 * x + d2);
  const T numerator =
      std::pow(f, d1 / static_cast<T>(2.0)) *
      std::pow(static_cast<T>(1.0) - f, d2 / static_cast<T>(2.0));
  const T denominator =
      x * BetaFunction(d1 / static_cast<T>(2.0), d2 / static_cast<T>(2.0));
  return numerator / denominator;
}

template <typename T>
T evalGeometric(T x, const Distribution::ParametersArray<T> params) {
  if (std::isinf(x)) {
    return NAN;
  }
  constexpr T castedOne = static_cast<T>(1.0);
  if (x < castedOne) {
    return static_cast<T>(0.0);
  }
  const T p = params[0];
  if (p == castedOne) {
    return x == castedOne ? castedOne : static_cast<T>(0.0);
  }
  // The result is p * (1-p)^{k-1}
  T lResult = (std::floor(x) - castedOne) * std::log(castedOne - p);
  return p * std::exp(lResult);
}

template <typename T>
T evalHyperGeometric(T k, const Distribution::ParametersArray<T> parameters) {
  const T N = parameters[Params::Hypergeometric::NPop];
  const T K = parameters[Params::Hypergeometric::K];
  const T n = parameters[Params::Hypergeometric::NSample];

  if (!std::isfinite(k) || n > N || K > N ||
      (N - K == N && K != static_cast<T>(0.))) {
    /* `N - K == N` checks if K is negligible compared to N, to avoid precision
     * errors. */
    return NAN;
  }
  k = std::floor(k);
  if (k < 0 || k > std::min(n, K)) {
    return 0;
  }
  /* We don't want BinomialCoefficient to generalize the formula
   * (K - k) > (N - n) is equivalent to (n - k) > (N - K), but numerically it
   * can give different results. The first comparison is the "understandable"
   * one, the second comparison is the same but subtracts values with same
   * magnitude. */
  if (k > K || (n - k) > (N - K) || (K - k) > (N - n)) {
    return 0;
  }
  T result = Approximation::FloatBinomial(K, k) *
             Approximation::FloatBinomial(N - K, n - k) /
             Approximation::FloatBinomial(N, n);
  if (result < 0 || result > 1) {
    // Precision errors
    return NAN;
  }
  return result;
}

template <typename T>
T evalNormal(T x, const Distribution::ParametersArray<T> params) {
  if (std::isinf(x)) {
    return NAN;
  }
  const T mu = params[Params::Normal::Mu];
  const T sigma = params[Params::Normal::Sigma];
  const float xMinusMuOverVar = (x - mu) / sigma;
  return (static_cast<T>(1.0)) /
         (std::fabs(sigma) * static_cast<T>(M_SQRT_2PI)) *
         std::exp(-(static_cast<T>(0.5)) * xMinusMuOverVar * xMinusMuOverVar);
}

template <typename T>
T evalPoisson(T x, const Distribution::ParametersArray<T> parameters) {
  if (x < 0 || std::isinf(x)) {
    return NAN;
  }
  const T lambda = parameters[0];
  T lResult = -lambda + std::floor(x) * std::log(lambda) -
              std::lgamma(std::floor(x) + 1);
  return std::exp(lResult);
}

template <typename T>
T evalStudent(T x, const Distribution::ParametersArray<T> params) {
  const T k = params[0];
  T lnCoefficient = std::lgamma((k + 1.f) / 2.f) - std::lgamma(k / 2.f) -
                    std::log(std::sqrt(k * M_PI));
  return std::exp(lnCoefficient - (k + 1.0) / 2.0 * std::log(1.0 + x * x / k));
}

template <typename T>
T evaluateUniform(T x, const Distribution::ParametersArray<T> params) {
  if (std::isinf(x)) {
    return NAN;
  }
  const T a = params[Params::Uniform::A];
  const T b = params[Params::Uniform::B];
  if (a <= x && x <= b) {
    return (1.0 / (b - a));
  }
  return 0.0;
}

template <typename T>
T EvaluateAtAbscissa(Type type, T x, const ParametersArray<T> parameters) {
  if (std::isnan(x) ||
      AreParametersValid(type, parameters) != OMG::Troolean::True) {
    return NAN;
  }
  switch (type) {
    case Type::Binomial:
      return evalBinomial(x, parameters);
    case Type::Uniform:
      return evaluateUniform(x, parameters);
    case Type::Exponential:
      return evalExponential(x, parameters);
    case Type::Normal:
      return evalNormal(x, parameters);
    case Type::Chi2:
      return evalChi2(x, parameters);
    case Type::Student:
      return evalStudent(x, parameters);
    case Type::Geometric:
      return evalGeometric(x, parameters);
    case Type::Hypergeometric:
      return evalHyperGeometric(x, parameters);
    case Type::Poisson:
      return evalPoisson(x, parameters);
    case Type::Fisher:
      return evalFischer(x, parameters);
    default:
      OMG::unreachable();
  }
}

template float EvaluateAtAbscissa(
    Type type, float x, const Distribution::ParametersArray<float> parameters);
template double EvaluateAtAbscissa(
    Type type, double x,
    const Distribution::ParametersArray<double> parameters);

}  // namespace Poincare::Internal::Distribution
