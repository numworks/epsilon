#include <omg/troolean.h>
#include <omg/unreachable.h>
#include <poincare/src/solver/solver_algorithms.h>
#include <poincare/statistics/distribution.h>

#include "binomial_distribution.h"
#include "chi2_distribution.h"
#include "continuous_distribution.h"
#include "discrete_distribution.h"
#include "exponential_distribution.h"
#include "fisher_distribution.h"
#include "geometric_distribution.h"
#include "hypergeometric_distribution.h"
#include "normal_distribution.h"
#include "poisson_distribution.h"
#include "student_distribution.h"
#include "uniform_distribution.h"

namespace Poincare::Internal::Distribution {

template <typename U>
OMG::Troolean IsParameterValid(Type type, U val, int index,
                               const ParametersArray<U> parameters) {
  assert(index >= 0 && index < NumberOfParameters(type));
  switch (type) {
    case Type::Binomial:
      return BinomialDistribution::IsParameterValid(val, index, parameters);
    case Type::Uniform:
      return UniformDistribution::IsParameterValid(val, index, parameters);
    case Type::Exponential:
      return ExponentialDistribution::IsParameterValid(val, index, parameters);
    case Type::Normal:
      return NormalDistribution::IsParameterValid(val, index, parameters);
    case Type::Chi2:
      return Chi2Distribution::IsParameterValid(val, index, parameters);
    case Type::Student:
      return StudentDistribution::IsParameterValid(val, index, parameters);
    case Type::Geometric:
      return GeometricDistribution::IsParameterValid(val, index, parameters);
    case Type::Hypergeometric:
      return HypergeometricDistribution::IsParameterValid(val, index,
                                                          parameters);
    case Type::Poisson:
      return PoissonDistribution::IsParameterValid(val, index, parameters);
    case Type::Fisher:
      return FisherDistribution::IsParameterValid(val, index, parameters);
    default:
      OMG::unreachable();
  }
}

template <typename U>
OMG::Troolean AreParametersValid(Type type,
                                 const ParametersArray<U> parameters) {
  int nParams = NumberOfParameters(type);
  OMG::Troolean result = OMG::Troolean::True;
  for (int i = 0; i < nParams; i++) {
    OMG::Troolean isParamValid =
        IsParameterValid(type, parameters[i], i, parameters);
    if (isParamValid == OMG::Troolean::False) {
      return OMG::Troolean::False;
    }
    if (isParamValid == OMG::Troolean::Unknown) {
      result = OMG::Troolean::Unknown;
    }
  }
  return result;
}

template <typename T>
T EvaluateAtAbscissa(Type type, T x, const ParametersArray<T> parameters) {
  if (!std::isfinite(x) ||
      AreParametersValid(type, parameters) != OMG::Troolean::True) {
    return NAN;
  }
  switch (type) {
    case Type::Binomial:
      return BinomialDistribution::EvaluateAtAbscissa(x, parameters);
    case Type::Uniform:
      return UniformDistribution::EvaluateAtAbscissa(x, parameters);
    case Type::Exponential:
      return ExponentialDistribution::EvaluateAtAbscissa(x, parameters);
    case Type::Normal:
      return NormalDistribution::EvaluateAtAbscissa(x, parameters);
    case Type::Chi2:
      return Chi2Distribution::EvaluateAtAbscissa(x, parameters);
    case Type::Student:
      return StudentDistribution::EvaluateAtAbscissa(x, parameters);
    case Type::Geometric:
      return GeometricDistribution::EvaluateAtAbscissa(x, parameters);
    case Type::Hypergeometric:
      return HypergeometricDistribution::EvaluateAtAbscissa(x, parameters);
    case Type::Poisson:
      return PoissonDistribution::EvaluateAtAbscissa(x, parameters);
    case Type::Fisher:
      return FisherDistribution::EvaluateAtAbscissa(x, parameters);
    default:
      OMG::unreachable();
  }
}

template <typename T>
T MeanAbscissa(Type type, const ParametersArray<T> parameters) {
  switch (type) {
    case Type::Normal:
      return NormalDistribution::MeanAbscissa(parameters);
    case Type::Student:
      return StudentDistribution::MeanAbscissa(parameters);
    case Type::Uniform:
      return UniformDistribution::MeanAbscissa(parameters);
    default:
      OMG::unreachable();
  }
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
      return BinomialDistribution::CumulativeDistributiveFunctionAtAbscissa(
          x, parameters);
    case Type::Uniform:
      return UniformDistribution::CumulativeDistributiveFunctionAtAbscissa(
          x, parameters);
    case Type::Exponential:
      return ExponentialDistribution::CumulativeDistributiveFunctionAtAbscissa(
          x, parameters);
    case Type::Normal:
      return NormalDistribution::CumulativeDistributiveFunctionAtAbscissa(
          x, parameters);
    case Type::Chi2:
      return Chi2Distribution::CumulativeDistributiveFunctionAtAbscissa(
          x, parameters);
    case Type::Student:
      return StudentDistribution::CumulativeDistributiveFunctionAtAbscissa(
          x, parameters);
    case Type::Fisher:
      return FisherDistribution::CumulativeDistributiveFunctionAtAbscissa(
          x, parameters);
    default:
      assert(!IsContinuous(type));
      return DiscreteDistribution::CumulativeDistributiveFunctionAtAbscissa<T>(
          type, x, parameters);
  }
}

template <typename T>
T CumulativeDistributiveInverseForProbability(
    Type type, T probability, const ParametersArray<T> parameters) {
  if (AreParametersValid(type, parameters) != OMG::Troolean::True ||
      !std::isfinite(probability) || probability < static_cast<T>(0.0) ||
      probability > static_cast<T>(1.0)) {
    return NAN;
  }
  switch (type) {
    case Type::Binomial:
      return BinomialDistribution::CumulativeDistributiveInverseForProbability(
          probability, parameters);
    case Type::Uniform:
      return UniformDistribution::CumulativeDistributiveInverseForProbability(
          probability, parameters);
    case Type::Exponential:
      return ExponentialDistribution::
          CumulativeDistributiveInverseForProbability(probability, parameters);
    case Type::Normal:
      return NormalDistribution::CumulativeDistributiveInverseForProbability(
          probability, parameters);
    case Type::Chi2:
      return Chi2Distribution::CumulativeDistributiveInverseForProbability(
          probability, parameters);
    case Type::Student:
      return StudentDistribution::CumulativeDistributiveInverseForProbability(
          probability, parameters);
    case Type::Geometric:
      return GeometricDistribution::CumulativeDistributiveInverseForProbability(
          probability, parameters);
    case Type::Hypergeometric:
      return HypergeometricDistribution::
          CumulativeDistributiveInverseForProbability(probability, parameters);
    case Type::Poisson:
      return PoissonDistribution::CumulativeDistributiveInverseForProbability(
          probability, parameters);
    case Type::Fisher:
      return FisherDistribution::CumulativeDistributiveInverseForProbability(
          probability, parameters);
    default:
      OMG::unreachable();
  }
}

template <typename T>
T CumulativeDistributiveFunctionForRange(Type type, T x, T y,
                                         const ParametersArray<T> parameters) {
  if (AreParametersValid(type, parameters) != OMG::Troolean::True ||
      std::isnan(x) || std::isnan(y)) {
    return NAN;
  }
  if (IsContinuous(type)) {
    return ContinuousDistribution::CumulativeDistributiveFunctionForRange<T>(
        type, x, y, parameters);
  }
  return DiscreteDistribution::CumulativeDistributiveFunctionForRange<T>(
      type, x, y, parameters);
}

double EvaluateParameterForProbabilityAndBound(
    Type type, int parameterIndex, const ParametersArray<double> parameters,
    double probability, double bound, bool isUpperBound) {
  assert(type == Type::Normal);
  return NormalDistribution::EvaluateParameterForProbabilityAndBound(
      parameterIndex, parameters, probability, bound, isUpperBound);
}

template OMG::Troolean IsParameterValid(
    Type type, float val, int index,
    const Distribution::ParametersArray<float> parameters);
template OMG::Troolean IsParameterValid(
    Type type, double val, int index,
    const Distribution::ParametersArray<double> parameters);
template OMG::Troolean IsParameterValid(
    Type type, const Tree* val, int index,
    const Distribution::ParametersArray<const Tree*> parameters);

template OMG::Troolean AreParametersValid(
    Type type, const Distribution::ParametersArray<float> parameters);
template OMG::Troolean AreParametersValid(
    Type type, const Distribution::ParametersArray<double> parameters);
template OMG::Troolean AreParametersValid(
    Type type, const Distribution::ParametersArray<const Tree*> parameters);

template float EvaluateAtAbscissa(
    Type type, float x, const Distribution::ParametersArray<float> parameters);
template double EvaluateAtAbscissa(
    Type type, double x,
    const Distribution::ParametersArray<double> parameters);

template float MeanAbscissa(
    Type type, const Distribution::ParametersArray<float> parameters);
template double MeanAbscissa(
    Type type, const Distribution::ParametersArray<double> parameters);

template float CumulativeDistributiveFunctionAtAbscissa(
    Type type, float x, const Distribution::ParametersArray<float> parameters);
template double CumulativeDistributiveFunctionAtAbscissa(
    Type type, double x,
    const Distribution::ParametersArray<double> parameters);

template float CumulativeDistributiveInverseForProbability(
    Type type, float probability,
    const Distribution::ParametersArray<float> parameters);
template double CumulativeDistributiveInverseForProbability(
    Type type, double probability,
    const Distribution::ParametersArray<double> parameters);

template float CumulativeDistributiveFunctionForRange(
    Type type, float x, float y,
    const Distribution::ParametersArray<float> parameters);
template double CumulativeDistributiveFunctionForRange(
    Type type, double x, double y,
    const Distribution::ParametersArray<double> parameters);

}  // namespace Poincare::Internal::Distribution
