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
  m_xRange.setMin(-k_displayLeftMarginRatio * xMax(), k_lowerMaxFloat, k_upperMaxFloat);
}

void CurveViewRange::roundAbscissa() {
  int roundedXMean = std::round(xCenter());
  float halfScreenWidth = ((float)Ion::Display::Width)/2.0f;
  float newXMin = roundedXMean - halfScreenWidth;
  float newXMax = roundedXMean + halfScreenWidth - 1.0f;
  float interestingXMin = m_delegate->interestingXMin();
  if (newXMin < interestingXMin) {
    newXMin = interestingXMin - k_displayLeftMarginRatio * (float)Ion::Display::Width;
    newXMax = newXMin + (float)Ion::Display::Width;
  }
  if (std::isnan(newXMin) || std::isnan(newXMax)) {
    return;
  }
  m_xRange.setMax(newXMax, k_lowerMaxFloat, k_upperMaxFloat);
  setXMin(newXMin);
}

void CurveViewRange::normalize() {
  float xMean = xCenter();
  float yMean = yCenter();

  // Compute the X
  float newXMin = xMean - NormalizedXHalfRange();
  float newXMax = xMean + NormalizedXHalfRange();
  float interestingXMin = m_delegate->interestingXMin();
  if (newXMin < interestingXMin) {
    newXMin = interestingXMin -k_displayLeftMarginRatio*2.0f*NormalizedXHalfRange();
    newXMax = newXMin + 2.0f*NormalizedXHalfRange();
  }
  if (!std::isnan(newXMin) && !std::isnan(newXMax)) {
    m_xRange.setMax(newXMax, k_lowerMaxFloat, k_upperMaxFloat);
    MemoizedCurveViewRange::protectedSetXMin(newXMin, k_lowerMaxFloat, k_upperMaxFloat);
  }

  // Compute the Y
  m_yAuto = false;
  float newYMin = yMean - NormalizedYHalfRange();
  float newYMax = clipped(yMean + NormalizedYHalfRange(), true);
  if (!std::isnan(newYMin) && !std::isnan(newYMax)) {
    m_yRange.setMax(newYMax, k_lowerMaxFloat, k_upperMaxFloat);
    MemoizedCurveViewRange::protectedSetYMin(newYMin, k_lowerMaxFloat, k_upperMaxFloat);
  }
}

void CurveViewRange::setTrigonometric() {
  float interestingXMin = m_delegate->interestingXMin();
  float interestingXRange = Preferences::sharedPreferences()->angleUnit() == Preferences::AngleUnit::Degree ? 1200.0f : 21.0f;
  m_xRange.setMax(interestingXMin + interestingXRange, k_lowerMaxFloat, k_upperMaxFloat);
  MemoizedCurveViewRange::protectedSetXMin(interestingXMin - k_displayLeftMarginRatio * interestingXRange, k_lowerMaxFloat, k_upperMaxFloat);

  m_yAuto = false;
  constexpr float y = 1.6f;
  m_yRange.setMax(y, k_lowerMaxFloat, k_upperMaxFloat);
  MemoizedCurveViewRange::protectedSetYMin(-y, k_lowerMaxFloat, k_upperMaxFloat);
}

void CurveViewRange::setDefault() {
  if (m_delegate == nullptr) {
    return;
  }
  m_yAuto = true;
  float interestingXMin = m_delegate->interestingXMin();
  float interestingXRange = m_delegate->interestingXHalfRange();
  m_xRange.setMax(interestingXMin + interestingXRange, k_lowerMaxFloat, k_upperMaxFloat);
  setXMin(interestingXMin - k_displayLeftMarginRatio * interestingXRange);
}

}
