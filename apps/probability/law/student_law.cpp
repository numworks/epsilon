#include "student_law.h"
#include "incomplete_beta_function.h"
#include "helper.h"
#include <cmath>

namespace Probability {

float StudentLaw::xMin() const {
  return -xMax();
}

float StudentLaw::xMax() const {
  return 5.0f;
}

float StudentLaw::yMax() const {
  return coefficient() * (1.0f + k_displayTopMarginRatio);
}

float StudentLaw::evaluateAtAbscissa(float x) const {
  const float d = m_parameter1;
  return coefficient() * std::pow(1+std::pow(x,2)/d, -(d+1)/2);
}

bool StudentLaw::authorizedValueAtIndex(float x, int index) const {
  return x >= FLT_EPSILON;
}

double StudentLaw::cumulativeDistributiveFunctionAtAbscissa(double x) const {
  if (x == 0) {
    return 0.5;
  }
  /* TODO There are some computation errors, where the probability falsly jumps to 1.
   * k = 0.001 and P(x < 42000000) (for 41000000 it is around 0.5)
   * k = 0.01 and P(x < 8400000) (for 41000000 it is around 0.6) */
  const float k = m_parameter1;
  const double sqrtXSquaredPlusK = std::sqrt(x*x + k);
  double t = (x + sqrtXSquaredPlusK) / (2.0 * sqrtXSquaredPlusK);
  return IncompleteBetaFunction(k/2.0, k/2.0, t);
}

double StudentLaw::cumulativeDistributiveInverseForProbability(double * probability) {
  if (*probability == 0.5) {
    return 0.0;
  }
  const double small = DBL_EPSILON;
  const double big = m_parameter1 >= 1 ? 50.0 : 50 / m_parameter1;
  double xmin = *probability < 0.5 ? -big : small;
  double xmax = *probability < 0.5 ? -small : big;
  return cumulativeDistributiveInverseForProbabilityUsingBrentRoots(probability, xmin, xmax);
}

float StudentLaw::coefficient() const {
  const float k = m_parameter1;
  const float lnOfResult = std::lgamma((k+1)/2) - std::lgamma(k/2) - (M_PI+k)/2;
  return std::exp(lnOfResult);
}

}
