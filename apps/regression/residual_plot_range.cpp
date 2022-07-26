#include "residual_plot_range.h"

namespace Regression {

void ResidualPlotRange::calibrate(double xMin, double xMax, double yMin, double yMax, KDCoordinate height, KDCoordinate bannerHeight) {
  assert(xMax >= xMin && yMax >= yMin);
  /* Handle cases where limits are too close or equal. They are shifted by both
   * k_minFloat (impactful on very small values) and abs(xMax)*k_relativeMargin
   * (impactful on large values) in order to properly display them. */
  if (xMax - xMin < Shared::Range1D::k_minFloat) {
    // Proportional model handles datasets with a single abscissa value
    double shift = std::abs(xMax) * k_relativeMargin + Shared::Range1D::k_minFloat;
    xMax += shift;
    xMin -= shift;
  }
  if (yMax - yMin < Shared::Range1D::k_minFloat) {
    // In addition to proportional models, residuals could also be very close
    double shift = std::abs(yMax) * k_relativeMargin + Shared::Range1D::k_minFloat;
    yMax += shift;
    yMin -= shift;
  }
  double xOffset = (xMax - xMin) * k_relativeMargin;
  protectedSetXMin(xMin - xOffset, Shared::Range1D::k_lowerMaxFloat, Shared::Range1D::k_upperMaxFloat, false);
  protectedSetXMax(xMax + xOffset, Shared::Range1D::k_lowerMaxFloat, Shared::Range1D::k_upperMaxFloat, true);

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

  protectedSetYMin(yRangeMin, Shared::Range1D::k_lowerMaxFloat, Shared::Range1D::k_upperMaxFloat, false);
  protectedSetYMax(yRangeMax, Shared::Range1D::k_lowerMaxFloat, Shared::Range1D::k_upperMaxFloat, true);
}

}
