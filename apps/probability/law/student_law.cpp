#include "student_law.h"
#include <cmath>
#include <float.h>

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
  return 0;
  //TODO
}

double StudentLaw::cumulativeDistributiveInverseForProbability(double * probability) {
  if (*probability >= 1.0) {
    return INFINITY;
  }
  if (*probability <= 0.0) {
    return -INFINITY;
  }
  return 0;
  //TODO
}

float StudentLaw::coefficient() const {
  const float k = m_parameter1;
  const float lnOfResult = std::lgamma((k+1)/2) - std::lgamma(k/2) - (M_PI+k)/2;
  return std::exp(lnOfResult);
}

}
