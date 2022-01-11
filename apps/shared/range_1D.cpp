#include "range_1D.h"
#include <assert.h>
#include <ion.h>
#include <poincare/ieee754.h>
#include <algorithm>

namespace Shared {

void Range1D::setMin(float f, float lowerMaxFloat, float upperMaxFloat) {
  m_min = checkedMin(f, &m_max, lowerMaxFloat, upperMaxFloat);
}

void Range1D::setMax(float f, float lowerMaxFloat, float upperMaxFloat) {
  m_max = checkedMax(f, &m_min, lowerMaxFloat, upperMaxFloat);
}

float Range1D::checkedValue(float value, float * otherValue, float lowerMaxFloat, float upperMaxFloat, bool isMax) {
  value = clipped(value, isMax, lowerMaxFloat, upperMaxFloat);
  if (std::isnan(value) || otherValue == nullptr) {
    return value;
  }
  float factor = isMax ? -1.0f : +1.0f;
  if (value < *otherValue == isMax || value == *otherValue) {
    *otherValue = value + factor * defaultRangeLengthFor(value);
  }
  if (std::fabs(*otherValue - value) < k_minFloat) {
    *otherValue = clipped(value + factor * k_minFloat, isMax, lowerMaxFloat, upperMaxFloat);
  }
  return value;
}


float Range1D::defaultRangeLengthFor(float position) {
  return std::pow(10.0f, Poincare::IEEE754<float>::exponentBase10(position)-1.0f);
}

float Range1D::clipped(float x, bool isMax, float lowerMaxFloat, float upperMaxFloat) {
  float maxF = isMax ? upperMaxFloat : lowerMaxFloat;
  float minF = isMax ? -lowerMaxFloat : -upperMaxFloat;
  return std::max(minF, std::min(x, maxF));
}

}
