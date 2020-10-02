#include "curve_view_range.h"
#include <cmath>
#include <ion.h>
#include <poincare/preferences.h>
#include <algorithm>

using namespace Shared;
using namespace Poincare;

namespace Sequence {

CurveViewRange::CurveViewRange(InteractiveCurveViewRangeDelegate * delegate) :
  InteractiveCurveViewRange(delegate)
{
  MemoizedCurveViewRange::protectedSetXMin(-k_displayLeftMarginRatio * xMax(), k_lowerMaxFloat, k_upperMaxFloat);
}

void CurveViewRange::normalize() {
  float xMean = xCenter();
  float yMean = yCenter();

  const float unit = std::max(xGridUnit(), yGridUnit());

  // Compute the X
  const float newXHalfRange = NormalizedXHalfRange(unit);
  float newXMin = xMean - newXHalfRange;
  float newXMax = xMean + newXHalfRange;
  float interestingXMin = m_delegate->interestingXMin();
  if (newXMin < interestingXMin) {
    newXMin = interestingXMin -k_displayLeftMarginRatio*2.0f*newXHalfRange;
    newXMax = newXMin + 2.0f*newXHalfRange;
  }
  if (!std::isnan(newXMin) && !std::isnan(newXMax)) {
    m_xRange.setMax(newXMax, k_lowerMaxFloat, k_upperMaxFloat);
    MemoizedCurveViewRange::protectedSetXMin(newXMin, k_lowerMaxFloat, k_upperMaxFloat);
  }

  // Compute the Y
  const float newYHalfRange = NormalizedYHalfRange(unit);
  float newYMin = yMean - newYHalfRange;
  float newYMax = clipped(yMean + newYHalfRange, true);
  if (!std::isnan(newYMin) && !std::isnan(newYMax)) {
    m_yRange.setMax(newYMax, k_lowerMaxFloat, k_upperMaxFloat);
    MemoizedCurveViewRange::protectedSetYMin(newYMin, k_lowerMaxFloat, k_upperMaxFloat);
  }
}

}
