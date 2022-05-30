#include "residual_plot_range.h"

namespace Regression {

void ResidualPlotRange::calibrate(double xMin, double xMax, double yMin, double yMax, KDCoordinate height, KDCoordinate bannerHeight) {
  if (std::fabs(yMax - yMin) < FLT_EPSILON) {
    assert(std::fabs(yMax) < FLT_EPSILON);
    yMax = 1.0;
    yMin = -1.0;
  }
  assert(xMax > xMin && yMax > yMin);

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
