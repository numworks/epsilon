#include "distribution.h"

#include <omg/troolean.h>
#include <omg/unreachable.h>
#include <poincare/src/solver/solver_algorithms.h>

namespace Poincare::Internal::Distribution {

Type GetType(const Tree* tree) {
  assert(tree->isDistribution());
  return tree->nodeValueBlock(1)->get<Type>();
}

template <typename T>
T MeanAbscissa(Type type, const ParametersArray<T> parameters) {
  switch (type) {
    case Type::Normal:
      return parameters[Params::Normal::Mu];
    case Type::Student:
      return 0.0;
    case Type::Uniform:
      return (parameters[Params::Uniform::A] + parameters[Params::Uniform::B]) /
             2.0;
    default:
      OMG::unreachable();
  }
}

double EvaluateParameterForProbabilityAndBound(
    Type type, int parameterIndex, const ParametersArray<double> parameters,
    double probability, double bound, bool isUpperBound) {
  // Only implemented for Normal Distribution
  assert(type == Type::Normal);

  if (std::isnan(probability)) {
    return NAN;
  }

  double mu = parameters[Params::Normal::Mu];
  double sigma = parameters[Params::Normal::Sigma];

  assert(probability >= 0.0 && probability <= 1.0);
  if (!isUpperBound) {
    probability = 1.0 - probability;
  }
  /* If X following (mu, sigma) is inferior to bound, then Z following (0, 1)
   * is inferior to (bound - mu)/sigma. */
  double abscissaForStandardDistribution =
      CumulativeDistributiveInverseForProbability(Type::Normal, probability,
                                                  {0.0, 1.0});
  if (parameterIndex == Params::Normal::Mu) {
    return bound - sigma * abscissaForStandardDistribution;
  }
  assert(parameterIndex == Params::Normal::Sigma);
  if (abscissaForStandardDistribution == 0.) {
    if (bound == mu) {
      // Return default value if there is an infinity of possible sigma
      return Distribution::DefaultParameterAtIndex(Distribution::Type::Normal,
                                                   Params::Normal::Sigma);
    }
    return NAN;
  }
  double result = (bound - mu) / abscissaForStandardDistribution;
  return result > 0.0 ? result : NAN;  // Sigma can't be negative or null
}

template <typename T>
T GetFisherDistributionMode(Type type, const ParametersArray<T> parameters) {
  // Only implemented for Fisher Distribution
  assert(type == Type::Fisher);

  const float d1 = parameters[0];
  if (d1 > 2.0f) {
    const float d2 = parameters[1];
    return (d1 - 2.0f) / d1 * d2 / (d2 + 2.0f);
  }
  return NAN;
}

template <typename T>
T ComputeXMin(Type type, const ParametersArray<T> parameters) {
  switch (type) {
    case Type::Uniform: {
      assert(parameters[1] >= parameters[0]);
      if (parameters[1] - parameters[0] < FLT_EPSILON) {
        // If parameter is too big, subtracting only 1.0 wouldn't do anything.
        return parameters[0] - 1.0f;
      }
      return parameters[0] - 0.6f * (parameters[1] - parameters[0]);
    }
    case Type::Normal: {
      assert(!std::isnan(parameters[0]) && !std::isnan(parameters[1]));
      if (parameters[1] == 0.0f) {
        return parameters[0] - 1.0f;
      }
      return parameters[0] - 5.0f * std::fabs(parameters[1]);
    }
    case Type::Student: {
      return -5.0f;
    }
    case Type::Binomial:
    case Type::Geometric:
    case Type::Exponential:
    case Type::Chi2:
    case Type::Hypergeometric:
    case Type::Poisson:
    case Type::Fisher:
      return 0.0f;
    default:
      OMG::unreachable();
  }
}

template <typename T>
T ComputeXMax(Type type, const ParametersArray<T> parameters) {
  switch (type) {
    case Type::Binomial:
      return parameters[0] > 0.0f ? parameters[0] : 1.0f;
    case Type::Uniform:
      if (parameters[1] - parameters[0] < FLT_EPSILON) {
        // If parameter is too big, adding only 1.0 wouldn't do anything.
        return parameters[0] + 1.0f;
      }
      return parameters[1] + 0.6f * (parameters[1] - parameters[0]);
    case Type::Normal:
      assert(!std::isnan(parameters[0]) && !std::isnan(parameters[1]));
      if (parameters[1] == 0.0f) {
        return parameters[0] + 1.0f;
      }
      return parameters[0] + 5.0f * std::fabs(parameters[1]);
    case Type::Student:
      return -ComputeXMin(type, parameters);
    case Type::Geometric:
      assert(parameters[0] != 0.0f);
      return 5.0f / parameters[0];
    case Type::Exponential: {
      assert(parameters[0] != 0.0f);
      float result = 5.0f / parameters[0];
      if (result <= FLT_EPSILON) {
        result = 1.0f;
      }
      if (std::isinf(result)) {
        /* Lower privateComputeXMax. It is used for drawing so the value is
         * not that important. */
        return 1.0f / parameters[0];
      }
      return result;
    }
    case Type::Chi2:
    case Type::Poisson:
      assert(parameters[0] != 0.0f);
      return (parameters[0] + 5.0f * std::sqrt(parameters[0]));
    case Type::Hypergeometric:
      return std::min(parameters[1], parameters[2]);
    case Type::Fisher:
      return 5.0f;
    default:
      OMG::unreachable();
  }
}

template <typename T>
T ComputeYMax(Type type, const ParametersArray<T> parameters) {
  switch (type) {
    case Type::Binomial: {
      int maxAbscissa = parameters[1] < 1.0f
                            ? (parameters[0] + 1) * parameters[1]
                            : parameters[0];
      float result = EvaluateAtAbscissa<T>(type, maxAbscissa, parameters);
      if (std::isnan(result) || result <= 0.0f) {
        result = 1.0f;
      }
      return result;
    }
    case Type::Uniform: {
      float result = parameters[1] - parameters[0] < FLT_EPSILON
                         ? k_uniformDiracMaximum
                         : 1.0f / (parameters[1] - parameters[0]);
      if (result <= 0.0f || std::isnan(result) || std::isinf(result)) {
        result = 1.0f;
      }
      return result;
    }
    case Type::Normal: {
      float maxAbscissa = parameters[0];
      float result = EvaluateAtAbscissa<T>(type, maxAbscissa, parameters);
      if (std::isnan(result) || result <= 0.0f) {
        result = 1.0f;
      }
      return result;
    }
    case Type::Student:
      return EvaluateAtAbscissa<T>(type, 0.0f, parameters);
    case Type::Geometric:
      // Tha distribution is max for x == 1
      return EvaluateAtAbscissa<T>(type, 1.0f, parameters);
    case Type::Exponential: {
      float result = parameters[0];
      if (result <= 0.0f || std::isnan(result)) {
        result = 1.0f;
      }
      if (result <= 0.0f) {
        result = 1.0f;
      }
      return result;
    }
    case Type::Chi2: {
      float result;
      if (parameters[0] / 2.0f <= 1.0f + FLT_EPSILON) {
        result = 0.5f;
      } else {
        result =
            EvaluateAtAbscissa<T>(type, parameters[0] - 1.0f, parameters) * 1.2;
      }
      return result;
    }
    case Type::Hypergeometric: {
      float mean = parameters[2] * parameters[1] / parameters[0];  // n * K / N
      float maximum =
          std::max(EvaluateAtAbscissa<T>(type, std::floor(mean), parameters),
                   EvaluateAtAbscissa<T>(type, std::ceil(mean), parameters));
      return maximum;
    }
    case Type::Poisson: {
      int maxAbscissa = (int)parameters[0];
      assert(maxAbscissa >= 0.0f);
      float result = EvaluateAtAbscissa<T>(type, maxAbscissa, parameters);
      if (result <= 0.0f) {
        result = 1.0f;
      }
      return result;
    }
    case Type::Fisher: {
      const float m = GetFisherDistributionMode(type, parameters);
      float max = std::isnan(m) ? k_fisherDefaultMax
                                : EvaluateAtAbscissa<T>(type, m, parameters);
      return std::isnan(max) ? k_fisherDefaultMax : max;
    }
    default:
      OMG::unreachable();
  }
}

template float MeanAbscissa(
    Type type, const Distribution::ParametersArray<float> parameters);
template double MeanAbscissa(
    Type type, const Distribution::ParametersArray<double> parameters);

template float GetFisherDistributionMode(
    Type type, const Distribution::ParametersArray<float> parameters);
template double GetFisherDistributionMode(
    Type type, const Distribution::ParametersArray<double> parameters);

template float ComputeXMin(
    Type type, const Distribution::ParametersArray<float> parameters);
template double ComputeXMin(
    Type type, const Distribution::ParametersArray<double> parameters);

template float ComputeXMax(
    Type type, const Distribution::ParametersArray<float> parameters);
template double ComputeXMax(
    Type type, const Distribution::ParametersArray<double> parameters);

template float ComputeYMax(
    Type type, const Distribution::ParametersArray<float> parameters);
template double ComputeYMax(
    Type type, const Distribution::ParametersArray<double> parameters);
}  // namespace Poincare::Internal::Distribution
