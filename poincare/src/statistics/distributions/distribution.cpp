#include <omg/troolean.h>
#include <omg/unreachable.h>
#include <poincare/src/solver/solver_algorithms.h>
#include <poincare/statistics/distribution.h>

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
  if (abscissaForStandardDistribution == 0) {
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

template float MeanAbscissa(
    Type type, const Distribution::ParametersArray<float> parameters);
template double MeanAbscissa(
    Type type, const Distribution::ParametersArray<double> parameters);

}  // namespace Poincare::Internal::Distribution
