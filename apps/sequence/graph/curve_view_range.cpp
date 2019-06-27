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
  int roundedXMean = std::round(xCenter());
  float halfScreenWidth = ((float)Ion::Display::Width)/2.0f;
  float newXMin = clipped(roundedXMean - halfScreenWidth, false);
  float newXMax = clipped(roundedXMean + halfScreenWidth - 1.0f, true);
  float interestingXMin = m_delegate->interestingXMin();
  if (newXMin < interestingXMin) {
    newXMin = interestingXMin - k_displayLeftMarginRatio * (float)Ion::Display::Width;
    newXMax = newXMin + (float)Ion::Display::Width;
  }
  if (std::isnan(newXMin) || std::isnan(newXMax)) {
    return;
  }
  m_xMax = newXMax;
  MemoizedCurveViewRange::setXMin(newXMin);
  if (m_delegate) {
    m_delegate->didChangeRange(this);
  }
}

void CurveViewRange::normalize() {
  float xMean = xCenter();
  float yMean = yCenter();

  // Compute the X
  float newXMin = clipped(xMean - NormalizedXHalfRange(), false);
  float newXMax = clipped(xMean + NormalizedXHalfRange(), true);
  float interestingXMin = m_delegate->interestingXMin();
  if (newXMin < interestingXMin) {
    newXMin = interestingXMin -k_displayLeftMarginRatio*2.0f*NormalizedXHalfRange();
    newXMax = newXMin + 2.0f*NormalizedXHalfRange();
  }
  if (!std::isnan(newXMin) && !std::isnan(newXMax)) {
    m_xMax = newXMax;
    MemoizedCurveViewRange::setXMin(newXMin);
  }

  // Compute the Y
  m_yAuto = false;
  float newYMin = clipped(yMean - NormalizedYHalfRange(), false);
  float newYMax = clipped(yMean + NormalizedYHalfRange(), true);
  if (!std::isnan(newYMin) && !std::isnan(newYMax)) {
    m_yMax = newYMax;
    MemoizedCurveViewRange::setYMin(newYMin);
  }
}

void CurveViewRange::setTrigonometric() {
  float interestingXMin = m_delegate->interestingXMin();
  float interestingXRange = Preferences::sharedPreferences()->angleUnit() == Preferences::AngleUnit::Degree ? 1200.0f : 21.0f;
  m_xMax = interestingXMin + interestingXRange;
  MemoizedCurveViewRange::setXMin(interestingXMin - k_displayLeftMarginRatio * interestingXRange);

  m_yAuto = false;
  constexpr float y = 1.6f;
  m_yMax = y;
  MemoizedCurveViewRange::setYMin(-y);
}

void CurveViewRange::setDefault() {
  if (m_delegate == nullptr) {
    return;
  }
  m_yAuto = true;
  float interestingXMin = m_delegate->interestingXMin();
  float interestingXRange = m_delegate->interestingXHalfRange();
  m_xMax = interestingXMin + interestingXRange;
  setXMin(interestingXMin - k_displayLeftMarginRatio * interestingXRange);
}

}
