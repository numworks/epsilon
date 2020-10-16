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
  Shared::InteractiveCurveViewRange::normalize();

  /* The X axis is not supposed to go into the negatives, save for a small
   * margin. However, after normalizing, it could be the case. We thus shift
   * the X range rightward to the origin. */
  float interestingXMin = m_delegate->interestingXMin();
  float xRange = xMax() - xMin();
  m_xRange.setMin(interestingXMin - k_displayLeftMarginRatio * xRange);
  setXMax(xMin() + xRange);
}

}
