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
#include "poincare/src/statistics/distributions/discrete_distribution.h"
#include "poisson_distribution.h"
#include "student_distribution.h"
#include "uniform_distribution.h"

namespace Poincare::Internal {

template <typename U>
OMG::Troolean Distribution::isParameterValid(U val, int index,
                                             const U* parameters) const {
  assert(index < numberOfParameters());
  switch (m_type) {
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
OMG::Troolean Distribution::areParametersValid(const U* parameters) const {
  int nParams = numberOfParameters();
  OMG::Troolean result = OMG::Troolean::True;
  for (int i = 0; i < nParams; i++) {
    OMG::Troolean isParamValid = isParameterValid(parameters[i], i, parameters);
    if (isParamValid == OMG::Troolean::False) {
      return OMG::Troolean::False;
    }
    if (isParamValid == OMG::Troolean::Unknown) {
      result = OMG::Troolean::Unknown;
    }
  }
  return result;
}

bool Distribution::isContinuous() const {
  switch (m_type) {
    case Type::Binomial:
    case Type::Geometric:
    case Type::Hypergeometric:
    case Type::Poisson:
      return false;
    default:
      return true;
  }
}

bool Distribution::isSymmetrical() const {
  switch (m_type) {
    case Type::Normal:
    case Type::Student:
    case Type::Uniform:
      return true;
    default:
      return false;
  }
}

template <typename T>
T Distribution::evaluateAtAbscissa(T x, const T* parameters) const {
  if (std::isnan(x) || std::isinf(x) ||
      areParametersValid(parameters) != OMG::Troolean::True) {
    return NAN;
  }
  switch (m_type) {
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
T Distribution::meanAbscissa(const T* parameters) const {
  switch (m_type) {
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
T Distribution::cumulativeDistributiveFunctionAtAbscissa(
    T x, const T* parameters) const {
  if (areParametersValid(parameters) != OMG::Troolean::True || std::isnan(x)) {
    return NAN;
  }

  switch (m_type) {
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
      assert(!isContinuous());
      return DiscreteDistribution::CumulativeDistributiveFunctionAtAbscissa<T>(
          m_type, x, parameters);
  }
}

template <typename T>
T Distribution::cumulativeDistributiveInverseForProbability(
    T probability, const T* parameters) const {
  if (areParametersValid(parameters) != OMG::Troolean::True ||
      std::isnan(probability) || std::isinf(probability) ||
      probability < static_cast<T>(0.0) || probability > static_cast<T>(1.0)) {
    return NAN;
  }
  switch (m_type) {
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
T Distribution::cumulativeDistributiveFunctionForRange(
    T x, T y, const T* parameters) const {
  if (isContinuous()) {
    return ContinuousDistribution::CumulativeDistributiveFunctionForRange<T>(
        m_type, x, y, parameters);
  }
  return DiscreteDistribution::CumulativeDistributiveFunctionForRange<T>(
      m_type, x, y, parameters);
}

double Distribution::evaluateParameterForProbabilityAndBound(
    int parameterIndex, const double* parameters, double probability,
    double bound, bool isUpperBound) const {
  assert(m_type == Type::Normal);
  return NormalDistribution::EvaluateParameterForProbabilityAndBound(
      parameterIndex, parameters, probability, bound, isUpperBound);
}

template OMG::Troolean Distribution::isParameterValid(
    float val, int index, const float* parameters) const;
template OMG::Troolean Distribution::isParameterValid(
    double val, int index, const double* parameters) const;
template OMG::Troolean Distribution::isParameterValid(
    const Tree* val, int index, const Tree* const* parameters) const;

template OMG::Troolean Distribution::areParametersValid(
    const float* parameters) const;
template OMG::Troolean Distribution::areParametersValid(
    const double* parameters) const;
template OMG::Troolean Distribution::areParametersValid(
    const Tree* const* parameters) const;

template float Distribution::evaluateAtAbscissa(float x,
                                                const float* parameters) const;
template double Distribution::evaluateAtAbscissa(
    double x, const double* parameters) const;

template float Distribution::meanAbscissa(const float* parameters) const;
template double Distribution::meanAbscissa(const double* parameters) const;

template float Distribution::cumulativeDistributiveFunctionAtAbscissa(
    float x, const float* parameters) const;
template double Distribution::cumulativeDistributiveFunctionAtAbscissa(
    double x, const double* parameters) const;

template float Distribution::cumulativeDistributiveInverseForProbability(
    float probability, const float* parameters) const;
template double Distribution::cumulativeDistributiveInverseForProbability(
    double probability, const double* parameters) const;

template float Distribution::cumulativeDistributiveFunctionForRange(
    float x, float y, const float* parameters) const;
template double Distribution::cumulativeDistributiveFunctionForRange(
    double x, double y, const double* parameters) const;

}  // namespace Poincare::Internal
