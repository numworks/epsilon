#include "student_distribution.h"

#include <cmath>

#include "probability/models/student_law.h"

namespace Probability {

float StudentDistribution::xMin() const {
  return -xMax();
}

float StudentDistribution::xMax() const {
  return 5.0f;
}

float StudentDistribution::yMax() const {
  return std::exp(StudentLaw::lnCoefficient<float>(m_parameter1)) * (1.0f + k_displayTopMarginRatio);
}

float StudentDistribution::evaluateAtAbscissa(float x) const {
  return Probability::StudentLaw::EvaluateAtAbscissa<float>(x, m_parameter1);
}

bool StudentDistribution::authorizedValueAtIndex(double x, int index) const {
  if (!OneParameterDistribution::authorizedValueAtIndex(x, index)) {
    return false;
  }
  return x >= DBL_EPSILON && x <= 200.0; // We cannot draw the curve for x > 200 (coefficient() is too small)
}

double StudentDistribution::cumulativeDistributiveFunctionAtAbscissa(double x) const {
  return Probability::StudentLaw::CumulativeDistributiveFunctionAtAbscissa<double>(x, m_parameter1);
}

double StudentDistribution::cumulativeDistributiveInverseForProbability(double * probability) {
  return Probability::StudentLaw::CumulativeDistributiveInverseForProbability<double>(*probability, m_parameter1);
}


}
