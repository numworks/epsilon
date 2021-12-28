#include "student_distribution.h"
#include <poincare/student_distribution.h>
#include <cmath>

namespace Probability {

float StudentDistribution::evaluateAtAbscissa(float x) const {
  return Poincare::StudentDistribution::EvaluateAtAbscissa<float>(x, m_parameter1);
}

bool StudentDistribution::authorizedValueAtIndex(double x, int index) const {
  if (!OneParameterDistribution::authorizedValueAtIndex(x, index)) {
    return false;
  }
  return x >= DBL_EPSILON && x <= 200.0; // We cannot draw the curve for x > 200 (coefficient() is too small)
}

double StudentDistribution::cumulativeDistributiveFunctionAtAbscissa(double x) const {
  return Poincare::StudentDistribution::CumulativeDistributiveFunctionAtAbscissa<double>(x, m_parameter1);
}

double StudentDistribution::cumulativeDistributiveInverseForProbability(double * probability) {
  return Poincare::StudentDistribution::CumulativeDistributiveInverseForProbability<double>(*probability, m_parameter1);
}

float StudentDistribution::computeXMin() const {
  return -computeXMax();
}

float StudentDistribution::computeXMax() const {
  return 5.0f;
}

float StudentDistribution::computeYMax() const {
  return std::exp(Poincare::StudentDistribution::lnCoefficient<float>(m_parameter1)) * (1.0f + k_displayTopMarginRatio);
}

}
