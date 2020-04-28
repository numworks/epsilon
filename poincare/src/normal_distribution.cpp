#include <poincare/normal_distribution.h>
#include <poincare/erf_inv.h>
#include <poincare/rational.h>
#include <cmath>
#include <float.h>
#include <assert.h>

#define M_SQRT_2PI 2.506628274631000502415765284811

namespace Poincare {

template<typename T>
T NormalDistribution::EvaluateAtAbscissa(T x, T mu, T sigma) {
  if (std::isnan(x) || std::isinf(x) || !MuAndSigmaAreOK(mu, sigma)){
    return NAN;
  }
  const float xMinusMuOverVar = (x - mu)/sigma;
  return ((T)1.0)/(std::fabs(sigma) * (T)M_SQRT_2PI) * std::exp(-((T)0.5) * xMinusMuOverVar * xMinusMuOverVar);
}

template<typename T>
T NormalDistribution::CumulativeDistributiveFunctionAtAbscissa(T x, T mu, T sigma) {
  if (!MuAndSigmaAreOK(mu, sigma)) {
    return NAN;
  }
  return StandardNormalCumulativeDistributiveFunctionAtAbscissa<T>((x-mu)/std::fabs(sigma));
}

template<typename T>
T NormalDistribution::CumulativeDistributiveInverseForProbability(T probability, T mu, T sigma) {
  if (!MuAndSigmaAreOK(mu, sigma)) {
    return NAN;
  }
  return StandardNormalCumulativeDistributiveInverseForProbability(probability) * std::fabs(sigma) + mu;
}

template<typename T>
bool NormalDistribution::MuAndSigmaAreOK(T mu, T sigma) {
  return !std::isnan(mu) && !std::isnan(sigma)
    && !std::isinf(mu) && !std::isinf(sigma)
    && sigma > (T)0.0;
}

bool NormalDistribution::ExpressionMuAndVarAreOK(bool * result, const Expression & mu, const Expression & var, Context * context) {
  assert(result != nullptr);
  if (mu.deepIsMatrix(context) || var.deepIsMatrix(context)) {
    *result = false;
    return true;
  }

  if (mu.isUndefined() || var.isUndefined() || Expression::IsInfinity(mu, context) || Expression::IsInfinity(var,context)) {
    *result = false;
    return true;
  }
  if (!mu.isReal(context) || !var.isReal(context)) {
    // We cannot check that mu and var are real
    return false;
  }

  {
    ExpressionNode::Sign s = var.sign(context);
    if (s == ExpressionNode::Sign::Negative) {
      *result = false;
      return true;
    }
    // We cannot check that var is positive
    if (s != ExpressionNode::Sign::Positive) {
      return false;
    }
  }

  if (var.type() != ExpressionNode::Type::Rational) {
    // We cannot check that var is not null
    return false;
  }

  const Rational rationalVar = static_cast<const Rational &>(var);
  if (rationalVar.isZero()) {
    *result = false;
    return true;
  }
  *result = true;
  return true;
}

template<typename T>
T NormalDistribution::StandardNormalCumulativeDistributiveFunctionAtAbscissa(T abscissa) {
  if (std::isnan(abscissa)) {
    return NAN;
  }
  if (std::isinf(abscissa) || std::fabs(abscissa) > k_boundStandardNormalDistribution) {
    return abscissa > (T)0.0 ? (T)1.0 : (T)0.0;
  }
  if (abscissa == (T)0.0) {
    return (T)0.5;
  }
  if (abscissa < (T)0.0) {
    return ((T)1.0) - StandardNormalCumulativeDistributiveFunctionAtAbscissa(-abscissa);
  }
  return ((T)0.5) + ((T)0.5) * std::erf(abscissa/(T)M_SQRT2);
}

template<typename T>
T NormalDistribution::StandardNormalCumulativeDistributiveInverseForProbability(T probability) {
  if (probability > (T)1.0 || probability < (T)0.0 || std::isnan(probability) || std::isinf(probability)) {
    return NAN;
  }
  T precision = sizeof(T) == sizeof(double) ? DBL_EPSILON : FLT_EPSILON;
  if (((T)1.0) - probability < precision) {
    return INFINITY;
  }
  if (probability < precision) {
    return -INFINITY;
  }
  if (probability < (T)0.5) {
    return -StandardNormalCumulativeDistributiveInverseForProbability(((T)1.0)-probability);
  }
  return (T)M_SQRT2 * erfInv(((T)2.0) * probability - (T)1.0);
}

template float NormalDistribution::EvaluateAtAbscissa<float>(float, float, float);
template double NormalDistribution::EvaluateAtAbscissa<double>(double, double, double);
template float NormalDistribution::CumulativeDistributiveFunctionAtAbscissa<float>(float, float, float);
template double NormalDistribution::CumulativeDistributiveFunctionAtAbscissa<double>(double, double, double);
template float NormalDistribution::CumulativeDistributiveInverseForProbability<float>(float, float, float);
template double NormalDistribution::CumulativeDistributiveInverseForProbability<double>(double, double, double);
template bool NormalDistribution::MuAndSigmaAreOK(float mu, float sigma);
template bool NormalDistribution::MuAndSigmaAreOK(double mu, double sigma);

}
