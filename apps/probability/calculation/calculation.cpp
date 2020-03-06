#include "calculation.h"
#include <assert.h>
#include <cmath>

namespace Probability {

void Calculation::setDistribution(Distribution * distribution) {
  m_distribution = distribution;
  compute(0);
}

double Calculation::lowerBound() {
  return -INFINITY;
}

double Calculation::upperBound() {
  return INFINITY;
}

}
