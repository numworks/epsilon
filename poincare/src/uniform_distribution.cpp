#include <poincare/uniform_distribution.h>
#include <poincare/beta_function.h>
#include <poincare/regularized_incomplete_beta_function.h>
#include <poincare/float.h>
#include <poincare/domain.h>
#include <cmath>
#include <float.h>
#include <assert.h>

namespace Poincare {

template<typename T>
T UniformDistribution::EvaluateAtAbscissa(T x, T d1, T d2) {
  if (std::isnan(x) || std::isinf(x) || !D1AndD2AreOK(d1, d2)){
    return NAN;
  }
  if (d1 <= x && x <= d2) {
    return (1.0/(d2 - d1));
  }
  return 0.0f;
}

template<typename T>
T UniformDistribution::CumulativeDistributiveFunctionAtAbscissa(T x, T d1, T d2) {
  if (!D1AndD2AreOK(d1, d2)) {
    return NAN;
  }
  if (x <= d1) {
    return 0.0;
  }
  if (x < d2) {
    return (x - d1)/(d2 - d1);
  }
  return 1.0;
}

template<typename T>
T UniformDistribution::CumulativeDistributiveInverseForProbability(T probability, T d1, T d2) {
  if (!D1AndD2AreOK(d1, d2)) {
    return NAN;
  }
  if (probability >= 1.0f) {
    return d2;
  }
  if (probability <= 0.0f) {
    return d1;
  }
  return d1 * (1 - probability) + probability * d2;
}

template<typename T>
bool UniformDistribution::D1AndD2AreOK(T d1, T d2) {
  return !std::isnan(d1) && !std::isnan(d2)
    && !std::isinf(d1) && !std::isinf(d2)
    && d1 <= d2;
}

bool UniformDistribution::ExpressionD1AndD2AreOK(bool * result, const Expression & d1, const Expression & d2, Context * context) {
  return Domain::expressionsAreIn(result, d2, Domain::Type::R, d1, Domain::Type::R, context);
}

template float UniformDistribution::EvaluateAtAbscissa<float>(float, float, float);
template double UniformDistribution::EvaluateAtAbscissa<double>(double, double, double);
template float UniformDistribution::CumulativeDistributiveFunctionAtAbscissa<float>(float, float, float);
template double UniformDistribution::CumulativeDistributiveFunctionAtAbscissa<double>(double, double, double);
template float UniformDistribution::CumulativeDistributiveInverseForProbability<float>(float, float, float);
template double UniformDistribution::CumulativeDistributiveInverseForProbability<double>(double, double, double);
template bool UniformDistribution::D1AndD2AreOK(float d1, float d2);
template bool UniformDistribution::D1AndD2AreOK(double d1, double d2);

}
