#include "range_1D.h"
#include <assert.h>
#include <ion.h>
#include <poincare/ieee754.h>
#include <algorithm>

namespace Shared {

void Range1D::setMin(float min, float lowerMaxFloat, float upperMaxFloat) {
  min = clipped(min, false, lowerMaxFloat, upperMaxFloat);
  if (std::isnan(min)) {
    return;
  }
  m_min = min;
  if (m_min >= m_max) {
    m_max = min + defaultRangeLengthFor(min);
  }
  if (m_max - min < k_minFloat) {
    m_max = clipped(min + k_minFloat, true, lowerMaxFloat, upperMaxFloat);
  }
}

void Range1D::setMax(float max, float lowerMaxFloat, float upperMaxFloat) {
  max = clipped(max, true, lowerMaxFloat, upperMaxFloat);
  if (std::isnan(max)) {
    return;
  }
  m_max = max;
  if (m_min >= m_max) {
    m_min = max - defaultRangeLengthFor(max);
  }
  if (max-m_min < k_minFloat) {
    m_min = clipped(max - k_minFloat, false, lowerMaxFloat, upperMaxFloat);
  }
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
