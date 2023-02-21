#include "curve_view_range.h"

#include <ion.h>
#include <poincare/preferences.h>

#include <algorithm>
#include <cmath>

#include "graph_controller.h"

using namespace Shared;
using namespace Poincare;

namespace Sequence {

CurveViewRange::CurveViewRange(GraphController *delegate)
    : InteractiveCurveViewRange(delegate) {}

void CurveViewRange::protectedNormalize(bool canChangeX, bool canChangeY,
                                        bool canShrink) {
  assert(m_delegate);
  Shared::InteractiveCurveViewRange::protectedNormalize(canChangeX, canChangeY,
                                                        canShrink);

  /* The X axis is not supposed to go into the negatives, save for a small
   * margin. However, after normalizing, it could be the case. We thus shift
   * the X range rightward to the origin. */
  GraphController *delegate = static_cast<GraphController *>(m_delegate);
  float interestingXMin = delegate->interestingXMin();
  float xRange = xMax() - xMin();
  float xMin = interestingXMin - k_displayLeftMarginRatio * xRange;
  assert(std::isfinite(xMin));
  protectedSetX(Range1D(xMin, xMin + xRange), k_maxFloat);
}

}  // namespace Sequence
