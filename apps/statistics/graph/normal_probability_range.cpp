#include "normal_probability_range.h"

namespace Statistics {

void NormalProbabilityRange::calibrate(KDCoordinate width, KDCoordinate height, float seriesXMin, float seriesXMax, float yMin, float yMax) {
  /* <                    width                          >
   * <   k_margin   >                     <   k_margin   >
   * ----------------|-------------------|----------------
   *             seriesXMin          seriesXMax
   * <  xSideOffset > <      xRange     > <  xSideOffset > */
  assert(seriesXMax - seriesXMin >= 0);
  float xRange = seriesXMax - seriesXMin;
  float yRange = yMax - yMin;
  assert(xRange >= 0 && yRange >= 0);

  float xSideOffset = xRange * static_cast<float>(k_margin) / static_cast<float>(width - 2*k_margin);
  float ySideOffset = yRange * static_cast<float>(k_margin) / static_cast<float>(height - 2*k_margin);

  MemoizedCurveViewRange::protectedSetXMin(seriesXMin - xSideOffset, INFINITY, INFINITY, false);
  MemoizedCurveViewRange::protectedSetXMax(seriesXMax + xSideOffset);
  MemoizedCurveViewRange::protectedSetYMin(yMin - ySideOffset, INFINITY, INFINITY, false);
  MemoizedCurveViewRange::protectedSetYMax(yMax + ySideOffset);
}

}
