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
      return parameters[0];
    case Type::Student:
      return 0.0;
    case Type::Uniform:
      return (parameters[0] + parameters[1]) / 2.0;
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
  assert(probability >= 0.0 && probability <= 1.0);
  if (!isUpperBound) {
    probability = 1.0 - probability;
  }
  /* If X following (mu, sigma) is inferior to bound, then Z following (0, 1)
   * is inferior to (bound - mu)/sigma. */
  double abscissaForStandardDistribution =
      CumulativeDistributiveInverseForProbability(Type::Normal, probability,
                                                  {0.0, 1.0});
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

template float MeanAbscissa(
    Type type, const Distribution::ParametersArray<float> parameters);
template double MeanAbscissa(
    Type type, const Distribution::ParametersArray<double> parameters);

}  // namespace Poincare::Internal::Distribution
