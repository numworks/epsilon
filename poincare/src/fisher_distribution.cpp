#include <poincare/fisher_distribution.h>
#include <poincare/beta_function.h>
#include <poincare/regularized_incomplete_beta_function.h>
#include <poincare/float.h>
#include <poincare/domain.h>
#include <cmath>
#include <float.h>
#include <assert.h>

namespace Poincare {

template<typename T>
T FisherDistribution::EvaluateAtAbscissa(T x, T d1, T d2) {
  if (std::isnan(x) || std::isinf(x) || !D1AndD2AreOK(d1, d2)){
    return NAN;
  }
  if (x < static_cast<T>(0.0)) {
    return NAN;
  }
  const T f = d1*x/(d1*x+d2);
  const T numerator = std::pow(f, d1/2.0) * std::pow(1.0 - f, d2/2.0);
  const T denominator = x * Poincare::BetaFunction(d1/2.0, d2/2.0);
  return numerator / denominator;
}

template<typename T>
T FisherDistribution::CumulativeDistributiveFunctionAtAbscissa(T x, T d1, T d2) {
  if (!D1AndD2AreOK(d1, d2)) {
    return NAN;
  }
  return Poincare::RegularizedIncompleteBetaFunction(d1/2.0, d2/2.0, d1*x/(d1*x+d2));
}

template<typename T>
T FisherDistribution::CumulativeDistributiveInverseForProbability(T probability, T d1, T d2) const {
  if (!D1AndD2AreOK(d1, d2)) {
    return NAN;
  }
  double parameters[2] = {d1, d2};
  return cumulativeDistributiveInverseForProbabilityUsingIncreasingFunctionRoot(probability, DBL_EPSILON, 100.0, parameters);  // Ad-hoc value;
}

template<typename T>
bool FisherDistribution::D1AndD2AreOK(T d1, T d2) {
  return !std::isnan(d1) && !std::isnan(d2)
    && !std::isinf(d1) && !std::isinf(d2)
    && d1 > static_cast<T>(0.0)
    && d2 > static_cast<T>(0.0);
}

bool FisherDistribution::ExpressionD1AndD2AreOK(bool * result, const Expression & d1, const Expression & d2, Context * context) {
  return Domain::ExpressionsAreIn(result, d1, Domain::Type::RPlusStar, d2, Domain::Type::RPlusStar, context);
}

template float FisherDistribution::EvaluateAtAbscissa<float>(float, float, float);
template double FisherDistribution::EvaluateAtAbscissa<double>(double, double, double);
template float FisherDistribution::CumulativeDistributiveFunctionAtAbscissa<float>(float, float, float);
template double FisherDistribution::CumulativeDistributiveFunctionAtAbscissa<double>(double, double, double);
template float FisherDistribution::CumulativeDistributiveInverseForProbability<float>(float, float, float) const;
template double FisherDistribution::CumulativeDistributiveInverseForProbability<double>(double, double, double) const;
template bool FisherDistribution::D1AndD2AreOK(float d1, float d2);
template bool FisherDistribution::D1AndD2AreOK(double d1, double d2);

}
