#include "chi_squared_law.h"
#include "regularized_gamma.h"
#include <poincare/solver.h>
#include <cmath>

namespace Probability {

float ChiSquaredLaw::xMin() const {
  return -k_displayLeftMarginRatio * xMax();
}

float ChiSquaredLaw::xMax() const {
  assert(m_parameter1 != 0);
  return (m_parameter1 + 5.0f * std::sqrt(m_parameter1)) * (1.0f + k_displayRightMarginRatio);
}

float ChiSquaredLaw::yMax() const {
  const float halfk = m_parameter1/2;
  float result;
  if (halfk <= 1 + FLT_EPSILON) {
    result = 0.5f;
  } else {
    result = coefficient() * std::pow(halfk - 1, halfk - 1);
  }
  return result * (1.0f + k_displayTopMarginRatio);
}

float ChiSquaredLaw::evaluateAtAbscissa(float x) const {
  if (x < 0) {
    return NAN;
  }
  const float halfk = m_parameter1/2;
  const float halfX = x/2;
  return coefficient() * std::pow(halfX, halfk-1) * std::exp(-halfX);
}

bool ChiSquaredLaw::authorizedValueAtIndex(float x, int index) const {
  return x >= FLT_EPSILON;
}

double ChiSquaredLaw::cumulativeDistributiveFunctionAtAbscissa(double x) const {
  if (x < 0) {
    return 0;
  }
  const float halfk = m_parameter1/2;
  double result = 0;
  if (regularizedGamma(halfk, x/2, k_regularizedGammaPrecision, k_maxRegularizedGammaIterations, &result)) {
    return result;
  }
  return NAN;
}

double ChiSquaredLaw::cumulativeDistributiveInverseForProbability(double * probability) {
  if (*probability >= 1) {
    return INFINITY;
  }
  if (*probability <= 0) {
    return 0;
  }
  Poincare::Coordinate2D result = Poincare::Solver::BrentMinimum(
      0.0,
      20.0,
      [](double x, Poincare::Context * context, Poincare::Preferences::ComplexFormat complexFormat, Poincare::Preferences::AngleUnit angleUnit, const void * context1, const void * context2, const void * context3) {
        const ChiSquaredLaw * law = reinterpret_cast<const ChiSquaredLaw *>(context1);
        const double * proba = reinterpret_cast<const double *>(context2);
        return std::fabs(law->cumulativeDistributiveFunctionAtAbscissa(x) - *proba);
      },
      nullptr,
      Poincare::Preferences::sharedPreferences()->complexFormat(),
      Poincare::Preferences::sharedPreferences()->angleUnit(),
      this,
      probability,
      nullptr);
  assert(std::fabs(result.value()) < FLT_EPSILON*10); // TODO FLT_EPSILON is too strict
  return result.abscissa();
}

float ChiSquaredLaw::coefficient() const {
  const float halfk = m_parameter1/2;
  return 1 / (2 * std::exp(std::lgamma(halfk)));
}

}
