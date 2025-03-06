#include <assert.h>
#include <omg/float.h>
#include <omg/troolean.h>
#include <omg/unreachable.h>
#include <poincare/src/solver/beta_function.h>
#include <poincare/src/solver/regularized_gamma_function.h>
#include <poincare/src/solver/regularized_incomplete_beta_function.h>
#include <poincare/src/solver/solver_algorithms.h>
#include <poincare/statistics/distribution.h>

namespace Poincare::Internal::Distribution {

template <typename T>
T binomialCumulativeDistributiveFunction(T x,
                                         const ParametersArray<T> parameters) {
  T n = parameters[Params::Binomial::N];
  T p = parameters[Params::Binomial::P];

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
T chi2CumulativeDistributiveFunction(T x, const ParametersArray<T> parameters) {
  if (x < DBL_EPSILON) {
    return 0.0;
  }

  constexpr static int k_maxRegularizedGammaIterations = 1000;
  constexpr static double k_regularizedGammaPrecision = DBL_EPSILON;

  const T k = parameters[0];
  double result = 0.0;
  if (RegularizedGammaFunction(k / 2.0, x / 2.0, k_regularizedGammaPrecision,
                               k_maxRegularizedGammaIterations, &result)) {
    return result;
  }
  return NAN;
}

template <typename T>
T exponentialCumulativeDistributiveFunction(T x,
                                            const ParametersArray<T> params) {
  if (x < 0.0) {
    return static_cast<T>(0.0);
  }
  const T lambda = params[0];
  return static_cast<T>(1.0) - std::exp((-lambda * x));
}

template <typename T>
T fischerCumulativeDistributiveFunction(T x, const ParametersArray<T> params) {
  const T d1 = params[Params::Fisher::D1];
  const T d2 = params[Params::Fisher::D2];
  return RegularizedIncompleteBetaFunction(d1 / 2.0, d2 / 2.0,
                                           d1 * x / (d1 * x + d2));
}

template <typename T>
static T standardNormalCumulativeDistributiveFunction(T abscissa) {
  if (std::isnan(abscissa)) {
    return NAN;
  }
  constexpr static T k_standardMu = DefaultParameterAtIndex(Type::Normal, 0);
  if (std::isinf(abscissa)) {
    return abscissa > k_standardMu ? static_cast<T>(1.0) : static_cast<T>(0.0);
  }
  if (abscissa == k_standardMu) {
    return static_cast<T>(0.5);
  }
  if (abscissa < k_standardMu) {
    return (static_cast<T>(1.0)) -
           standardNormalCumulativeDistributiveFunction(-abscissa);
  }
  return (static_cast<T>(0.5)) +
         (static_cast<T>(0.5)) * std::erf(abscissa / static_cast<T>(M_SQRT2));
}

template <typename T>
T normalCumulativeDistributiveFunction(T x, const ParametersArray<T> params) {
  const T mu = params[Params::Normal::Mu];
  const T sigma = params[Params::Normal::Sigma];
  return standardNormalCumulativeDistributiveFunction<T>((x - mu) /
                                                         std::fabs(sigma));
}

template <typename T>
T studentCumulativeDistributiveFunction(T x, const ParametersArray<T> params) {
  const T k = params[0];
  if (x == 0.0) {
    return static_cast<T>(0.5);
  }
  if (std::isinf(x)) {
    return x > 0 ? static_cast<T>(1.0) : static_cast<T>(0.0);
  }
  /* TODO There are some computation errors, where the probability falsly jumps
   * to 1. k = 0.001 and P(x < 42000000) (for 41000000 it is around 0.5) k =
   * 0.01 and P(x < 8400000) (for 41000000 it is around 0.6) */
  const double sqrtXSquaredPlusK = std::sqrt(x * x + k);
  double t = (x + sqrtXSquaredPlusK) / (2.0 * sqrtXSquaredPlusK);
  return RegularizedIncompleteBetaFunction(k / 2.0, k / 2.0, t);
}

template <typename T>
T uniformCumulativeDistributiveFunction(T x, const ParametersArray<T> params) {
  const T a = params[Params::Uniform::A];
  const T b = params[Params::Uniform::B];
  if (x <= a) {
    return 0.0;
  }
  if (x < b) {
    return (x - a) / (b - a);
  }
  return 1.0;
}

template <typename T>
T discreteCumulativeDistributiveFunction(Type distribType, T x,
                                         const ParametersArray<T> parameters) {
  if (std::isinf(x)) {
    return x > static_cast<T>(0.0) ? static_cast<T>(1.0) : static_cast<T>(0.0);
  }
  if (x < static_cast<T>(0.0)) {
    return static_cast<T>(0.0);
  }
  const void* pack[2] = {&distribType, &parameters};
  return SolverAlgorithms::CumulativeDistributiveFunctionForNDefinedFunction<T>(
      x,
      [](T k, const void* auxiliary) {
        const void* const* pack = static_cast<const void* const*>(auxiliary);
        Type* type = const_cast<Type*>(static_cast<const Type*>(pack[0]));
        const ParametersArray<T>* parameters =
            static_cast<const ParametersArray<T>*>(pack[1]);
        return EvaluateAtAbscissa(*type, k, *parameters);
      },
      pack);
}

template <typename T>
T CumulativeDistributiveFunctionAtAbscissa(
    Type type, T x, const ParametersArray<T> parameters) {
  if (AreParametersValid(type, parameters) != OMG::Troolean::True ||
      std::isnan(x)) {
    return NAN;
  }

  switch (type) {
    case Type::Binomial:
      return binomialCumulativeDistributiveFunction(x, parameters);
    case Type::Uniform:
      return uniformCumulativeDistributiveFunction(x, parameters);
    case Type::Exponential:
      return exponentialCumulativeDistributiveFunction(x, parameters);
    case Type::Normal:
      return normalCumulativeDistributiveFunction(x, parameters);
    case Type::Chi2:
      return chi2CumulativeDistributiveFunction(x, parameters);
    case Type::Student:
      return studentCumulativeDistributiveFunction(x, parameters);
    case Type::Fisher:
      return fischerCumulativeDistributiveFunction(x, parameters);
    default:
      assert(!IsContinuous(type));
      return discreteCumulativeDistributiveFunction(type, x, parameters);
  }
}

// The range is inclusive on both ends
template <typename T>
T discreteCumulativeDistributiveFunctionForRange(
    Type distribType, T x, T y, const ParametersArray<T> parameters) {
  if (y < x) {
    return 0.0f;
  }
  return CumulativeDistributiveFunctionAtAbscissa(distribType, y, parameters) -
         CumulativeDistributiveFunctionAtAbscissa(distribType, x - 1.0f,
                                                  parameters);
}

template <typename T>
T continuousCumulativeDistributiveFunctionForRange(
    Type distribType, T x, T y, const ParametersArray<T> parameters) {
  if (y <= x) {
    return 0.0f;
  }
  return CumulativeDistributiveFunctionAtAbscissa(distribType, y, parameters) -
         CumulativeDistributiveFunctionAtAbscissa(distribType, x, parameters);
}

template <typename T>
T CumulativeDistributiveFunctionForRange(Type type, T x, T y,
                                         const ParametersArray<T> parameters) {
  if (AreParametersValid(type, parameters) != OMG::Troolean::True ||
      std::isnan(x) || std::isnan(y)) {
    return NAN;
  }
  if (IsContinuous(type)) {
    return continuousCumulativeDistributiveFunctionForRange(type, x, y,
                                                            parameters);
  }
  return discreteCumulativeDistributiveFunctionForRange(type, x, y, parameters);
}

template float CumulativeDistributiveFunctionAtAbscissa(
    Type type, float x, const ParametersArray<float> parameters);
template double CumulativeDistributiveFunctionAtAbscissa(
    Type type, double x, const ParametersArray<double> parameters);

template float CumulativeDistributiveFunctionForRange(
    Type type, float x, float y, const ParametersArray<float> parameters);
template double CumulativeDistributiveFunctionForRange(
    Type type, double x, double y, const ParametersArray<double> parameters);

}  // namespace Poincare::Internal::Distribution
