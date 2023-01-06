#include "residual_plot_range.h"

namespace Regression {

void ResidualPlotRange::calibrate(double xMin, double xMax, double yMin, double yMax, KDCoordinate height, KDCoordinate bannerHeight) {
  Poincare::Range1D xRange(xMin, xMax);
  Poincare::Range1D yRange(yMin, yMax);

  /* Proportional model handles datasets with a single abscissa value. Residuals
   * y values could also be very close. They are shifted by both k_minLength
   * (impactful on very small values) and abs(max) * k_relativeMargin
   * (impactful on large values) in order to properly display them. */
  xRange.stretchIfTooSmall(std::fabs(xRange.max()) * k_relativeMargin + Poincare::Range1D::k_minLength);
  yRange.stretchIfTooSmall(std::fabs(yRange.max()) * k_relativeMargin + Poincare::Range1D::k_minLength);

  xRange.stretch(xRange.length() * k_relativeMargin);
  yRange.stretch(yRange.length() * k_relativeMargin);

  /* Banner hides small values of yRange, so reduce yMin */
  double heightRatio = static_cast<double>(bannerHeight) / static_cast<double>(height);
  double yRangeMin = (yRange.min() - yRange.max() * heightRatio) / (1 - heightRatio);
  yRange.setMin(yRangeMin);

  protectedSetX(xRange);
  protectedSetY(yRange);
}

}
