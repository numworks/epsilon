#include "interactive_curve_view_range_delegate.h"
#include "interactive_curve_view_range.h"
#include <cmath>
#include <float.h>

namespace Shared {

bool InteractiveCurveViewRangeDelegate::didChangeRange(InteractiveCurveViewRange * interactiveCurveViewRange) {
  /*TODO : De we want to keep this method ?
   * We might want to put some computations in here, like the new Auto and Normailzed statuses. */
  return false;
}

}
