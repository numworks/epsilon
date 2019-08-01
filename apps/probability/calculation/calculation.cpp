#include "calculation.h"
#include <assert.h>
#include <cmath>

namespace Probability {

void Calculation::setLaw(Law * law) {
  m_law = law;
  compute(0);
}

double Calculation::lowerBound() {
  return -INFINITY;
}

double Calculation::upperBound() {
  return INFINITY;
}

}
