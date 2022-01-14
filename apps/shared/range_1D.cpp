#include "range_1D.h"
#include <assert.h>
#include <ion.h>
#include <poincare/ieee754.h>
#include <algorithm>

namespace Shared {

void Range1D::setMin(float f, float lowerMaxFloat, float upperMaxFloat) {
  /* We can't use &m_max directly: Range1D is 'packed' so &m_max address might
   * be unaligned. It can be off by a few bits and not referable by an actual
   * address. */
  float max = m_max;
  m_min = checkedMin(f, &max, lowerMaxFloat, upperMaxFloat);
  m_max = max;
}

void Range1D::setMax(float f, float lowerMaxFloat, float upperMaxFloat) {
  // Same comment as setMin
  float min = m_min;
  m_max = checkedMax(f, &min, lowerMaxFloat, upperMaxFloat);
  m_min = min;
}

float Range1D::checkedValue(float value, float * otherValue, float lowerMaxFloat, float upperMaxFloat, bool isMax) {
  value = clipped(value, isMax, lowerMaxFloat, upperMaxFloat);
  if (std::isnan(value) || otherValue == nullptr) {
    return value;
  }
  float factor = isMax ? -1.0f : +1.0f;
  if ((value < *otherValue) == isMax || value == *otherValue) {
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
