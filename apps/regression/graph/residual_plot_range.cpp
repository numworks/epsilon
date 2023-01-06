#include "residual_plot_range.h"

namespace Regression {

void ResidualPlotRange::calibrate(double xMin, double xMax, double yMin, double yMax, KDCoordinate height, KDCoordinate bannerHeight) {
  calibrateXorY(true, xMin, xMax);
  calibrateXorY(false, yMin, yMax, height, bannerHeight);
}

void ResidualPlotRange::calibrateXorY(bool isX, double min, double max, KDCoordinate height, KDCoordinate bannerHeight) {
  Poincare::Range1D range(min, max);

  /* Proportional model handles datasets with a single abscissa value. Residuals
   * y values could also be very close. They are shifted by both k_minLength
   * (impactful on very small values) and abs(max) * k_relativeMargin
   * (impactful on large values) in order to properly display them. */
  range.stretchIfTooSmall(std::fabs(range.max()) * k_relativeMargin + Poincare::Range1D::k_minLength);

  // Stretch range for margins
  range.stretch(range.length() * k_relativeMargin);

  if (!isX) {
    // Banner hides small values of yRange, so reduce yMin
    double heightRatio = static_cast<double>(bannerHeight) / static_cast<double>(height);
    double rangeMin = (range.min() - range.max() * heightRatio) / (1 - heightRatio);
    range.setMin(rangeMin);
    protectedSetY(range);
  } else {
    protectedSetX(range);
  }
}

}
