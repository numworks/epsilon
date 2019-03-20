#include "curve_view_range.h"
#include <cmath>
#include <ion.h>
#include <poincare/preferences.h>

using namespace Shared;
using namespace Poincare;

namespace Sequence {

CurveViewRange::CurveViewRange(InteractiveCurveViewRangeDelegate * delegate) :
  InteractiveCurveViewRange(delegate)
{
  m_xMin = -k_displayLeftMarginRatio * m_xMax;
}

void CurveViewRange::roundAbscissa() {
  int roundedXMean = std::round((m_xMin+m_xMax)/2);
  float halfScreenWidth = ((float)Ion::Display::Width)/2.0f;
  float newXMin = clipped(roundedXMean - halfScreenWidth, false);
  float newXMax = clipped(roundedXMean + halfScreenWidth - 1.0f, true);
  if (std::isnan(newXMin) || std::isnan(newXMax)) {
    return;
  }
  m_xMin = newXMin;
  m_xMax = newXMax;
  float interestingXMin = m_delegate->interestingXMin();
  if (m_xMin < interestingXMin) {
    m_xMin = interestingXMin - k_displayLeftMarginRatio * (float)Ion::Display::Width;
    m_xMax = m_xMin + (float)Ion::Display::Width;
  }
  m_xGridUnit = computeGridUnit(Axis::X, m_xMax - m_xMin);
  if (m_delegate) {
    m_delegate->didChangeRange(this);
  }
}

void CurveViewRange::normalize() {
  float xMean = (m_xMin+m_xMax)/2;
  float yMean = (m_yMin+m_yMax)/2;

  // Compute the X
  float newXMin = clipped(xMean - NormalizedXHalfRange(), false);
  float newXMax = clipped(xMean + NormalizedXHalfRange(), true);
  if (!std::isnan(newXMin) && !std::isnan(newXMax)) {
    m_xMin = newXMin;
    m_xMax = newXMax;
    m_xGridUnit = computeGridUnit(Axis::X, m_xMax - m_xMin);
  }
  float interestingXMin = m_delegate->interestingXMin();
  if (m_xMin < interestingXMin) {
    m_xMin = interestingXMin -k_displayLeftMarginRatio*2.0f*NormalizedXHalfRange();
    m_xMax = m_xMin + 2.0f*NormalizedXHalfRange();
  }

  // Compute the Y
  m_yAuto = false;
  float newYMin = clipped(yMean - NormalizedYHalfRange(), false);
  float newYMax = clipped(yMean + NormalizedYHalfRange(), true);
  if (!std::isnan(newYMin) && !std::isnan(newYMax)) {
    m_yMin = newYMin;
    m_yMax = newYMax;
    m_yGridUnit = computeGridUnit(Axis::Y, m_yMax - m_yMin);
  }
}

void CurveViewRange::setTrigonometric() {
  float interestingXMin = m_delegate->interestingXMin();
  float interestingXRange = Preferences::sharedPreferences()->angleUnit() == Preferences::AngleUnit::Degree ? 1200.0f : 21.0f;
  m_xMin = interestingXMin - k_displayLeftMarginRatio * interestingXRange;
  m_xMax = interestingXMin + interestingXRange;
  m_xGridUnit = computeGridUnit(Axis::X, m_xMax - m_xMin);

  m_yAuto = false;
  constexpr float y = 1.6f;
  m_yMin = -y;
  m_yMax = y;
  m_yGridUnit = computeGridUnit(Axis::Y, m_yMax - m_yMin);
}

void CurveViewRange::setDefault() {
  if (m_delegate == nullptr) {
    return;
  }
  float interestingXMin = m_delegate->interestingXMin();
  float interestingXRange = m_delegate->interestingXHalfRange();
  m_xMin = interestingXMin - k_displayLeftMarginRatio * interestingXRange;
  m_xMax = interestingXMin + interestingXRange;
  m_xGridUnit = computeGridUnit(Axis::X, m_xMax - m_xMin);
  setYAuto(true);
}

}
