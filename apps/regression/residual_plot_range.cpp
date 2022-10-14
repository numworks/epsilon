#include "residual_plot_range.h"

namespace Regression {

void ResidualPlotRange::calibrate(double xMin, double xMax, double yMin, double yMax, KDCoordinate height, KDCoordinate bannerHeight) {
  assert(xMax >= xMin && yMax >= yMin);
  /* Proportional model handles datasets with a single abscissa value. residuals
   * y values could also be very close. */
  stretchRangeIfTooClose(&xMin, &xMax);
  stretchRangeIfTooClose(&yMin, &yMax);
  double xOffset = (xMax - xMin) * k_relativeMargin;
  protectedSetXMin(xMin - xOffset, false);
  protectedSetXMax(xMax + xOffset);

  /* Computing yRangeMax and yRangeMin so that :
   * - yRangeMax = yMax + k_relativeMargin * range = yRangeMin + height
   * - yMax = yMin + range
   * - yMin = yVisibleRangeMin + k_relativeMargin * range
   * - yVisibleRangeMin = yRangeMin + bannerHeight
   */

  double range = yMax - yMin;
  double yRangeMax = yMax + k_relativeMargin * range;
  double yVisibleRangeMin = yMin - k_relativeMargin * range;
  double heightRatio = static_cast<double>(bannerHeight) / static_cast<double>(height);
  double yRangeMin = (yVisibleRangeMin - yRangeMax * heightRatio) / (1 - heightRatio);

  protectedSetYMin(yRangeMin, false);
  protectedSetYMax(yRangeMax);
}

void ResidualPlotRange::stretchRangeIfTooClose(double * min, double * max) const {
  /* Handle cases where limits are too close or equal. They are shifted by both
   * k_minFloat (impactful on very small values) and abs(max)*k_relativeMargin
   * (impactful on large values) in order to properly display them. */
  if (*max - *min >= Poincare::Range1D::k_minLength) {
    return;
  }
  double shift = std::fabs(*max) * k_relativeMargin + Poincare::Range1D::k_minLength;
  *max += shift;
  *min -= shift;
  assert(*max - *min >= Poincare::Range1D::k_minLength);
}

}
