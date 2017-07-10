#include "calculation.h"
#include <assert.h>
#include <math.h>

namespace Probability {

Calculation::Calculation():
  m_law(nullptr)
{
}

void Calculation::setLaw(Law * law) {
  m_law = law;
  compute(0);
}

int Calculation::numberOfEditableParameters() {
  return numberOfParameters();
}

float Calculation::lowerBound() {
  return -INFINITY;
}

float Calculation::upperBound() {
  return INFINITY;
}

}
