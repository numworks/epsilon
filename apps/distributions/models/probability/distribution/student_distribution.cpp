#include "student_distribution.h"
#include <poincare/student_distribution.h>
#include <cmath>

namespace Inference {

using namespace Poincare;

bool StudentDistribution::authorizedParameterAtIndex(double x, int index) const {
  if (!OneParameterDistribution::authorizedParameterAtIndex(x, index)) {
    return false;
  }
  return x >= DBL_EPSILON && x <= 200.0; // We cannot draw the curve for x > 200 (coefficient() is too small)
}

float StudentDistribution::computeXMin() const {
  return -computeXMax();
}

float StudentDistribution::computeXMax() const {
  return 5.0f;
}

float StudentDistribution::computeYMax() const {
  return std::exp(Poincare::StudentDistribution::lnCoefficient<float>(m_parameter)) * (1.0f + k_displayTopMarginRatio);
}

}
