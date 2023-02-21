#include "plot_range.h"

namespace Statistics {

void PlotRange::calibrate(KDCoordinate width, KDCoordinate height,
                          KDCoordinate horizontalMargin,
                          KDCoordinate bottomMargin, KDCoordinate topMargin,
                          float seriesXMin, float seriesXMax, float yMin,
                          float yMax) {
  /* <                        width                              >
   * < horizontalMargin >                     < horizontalMargin >
   * --------------------|-------------------|--------------------
   *                 seriesXMin          seriesXMax
   * <    xSideOffset   > <      xRange     > <    xSideOffset   > */
  assert(seriesXMax - seriesXMin >= 0);
  float xRange = seriesXMax - seriesXMin;
  float yRange = yMax - yMin;
  assert(xRange >= 0 && yRange >= 0);

  float xSideOffset = xRange * static_cast<float>(horizontalMargin) /
                      static_cast<float>(width - 2 * horizontalMargin);
  float yTopOffset = yRange * static_cast<float>(topMargin) /
                     static_cast<float>(height - bottomMargin - topMargin);
  float yBottomOffset = yRange * static_cast<float>(bottomMargin) /
                        static_cast<float>(height - bottomMargin - topMargin);

  // Offset may be null because of equal bounds. Use it or 1.0f by default.
  if (xRange == 0.0f) {
    xSideOffset = (seriesXMin != 0.0f ? std::fabs(seriesXMin) : 1.0f);
  }
  if (yRange == 0.0f) {
    yTopOffset = yBottomOffset = (yMin != 0.0f ? std::fabs(yMin) : 1.0f);
  }

  protectedSetX(
      Poincare::Range1D(seriesXMin - xSideOffset, seriesXMax + xSideOffset),
      INFINITY);
  protectedSetY(Poincare::Range1D(yMin - yBottomOffset, yMax + yTopOffset),
                INFINITY);
}

}  // namespace Statistics
