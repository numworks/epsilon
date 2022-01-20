#ifndef POINCARE_ZOOM_H
#define POINCARE_ZOOM_H

#include <poincare/context.h>
#include <ion/display.h>

/* FIXME : This class is concerned with manipulating the ranges of graphing
 * window, often represented by four float xMin, xMax, yMin, yMax. Handling
 * those same four values has proven repetitive, tedious, and prone to error.
 * This code could benefit from a data structure to regroup those values in a
 * single object. */

namespace Poincare {

class Zoom {
public:
  static constexpr float k_defaultHalfRange = 10.f;
  static constexpr float k_smallUnitMantissa = 1.f;
  static constexpr float k_mediumUnitMantissa = 2.f;
  static constexpr float k_largeUnitMantissa = 5.f;
  static constexpr float k_minimalRangeLength = 1e-4f;

  typedef float (*ValueAtAbscissa)(float abscissa, Context * context, const void * auxiliary);

  /* Find the most suitable window to display the function's points of
   * interest. Return false if the X range was given a default value because
   * there were no points of interest. */
  static bool InterestingRangesForDisplay(ValueAtAbscissa evaluation, float * xMin, float * xMax, float * yMin, float * yMax, float tMin, float tMax, Context * context, const void * auxiliary);
  /* Find the best Y range to display the function on [xMin, xMax], but crop
   * the values that are outside of the function's order of magnitude. */
  static void RefinedYRangeForDisplay(ValueAtAbscissa evaluation, float * xMin, float * xMax, float * yMin, float * yMax, Context * context, const void * auxiliary);
  /* Find the best window to display functions, with a specified ratio
   * between X and Y. Usually used to find the most fitting orthonormal range. */
  static void RangeWithRatioForDisplay(ValueAtAbscissa evaluation, float yxRatio, float * xMin, float * xMax, float * yMin, float * yMax, Context * context, const void * auxiliary);
  static void FullRange(ValueAtAbscissa evaluation, float tMin, float tMax, float tStep, float * fMin, float * fMax, Context * context, const void * auxiliary);

  /* Find the bounding box of the given ranges. */
  static void CombineRanges(int length, const float * mins, const float * maxs, float * minRes, float * maxRes);
  /* Ensures that the window is fit for display, with all bounds being proper
   * numbers, with min < max. */
  static void SanitizeRange(float * xMin, float * xMax, float * yMin, float * yMax, float normalRatio);

  /* If shrink is false, the range will be set to ratio by increasing the size
   * of the smallest axis. If it is true, the longest axis will be reduced.*/
  static void SetToRatio(float yxRatio, float * xMin, float * xMax, float * yMin, float * yMax, bool shrink = false);
  static void SetZoom(float ratio, float xCenter, float yCenter, float * xMin, float * xMax, float * yMin, float * yMax);

private:
  static constexpr int k_peakNumberOfPointsOfInterest = 3;
  static constexpr int k_sampleSize = Ion::Display::Width / 4;
  static constexpr float k_maximalDistance = 1e5f;
  static constexpr float k_minimalDistance = 1e-2f;
  static constexpr float k_asymptoteThreshold = 2e-1f;
  static constexpr float k_explosionThreshold = 1e1f;
  static constexpr float k_stepFactor = 1.09f;
  static constexpr float k_breathingRoom = 0.3f;
  static constexpr float k_forceXAxisThreshold = 0.2f;
  static constexpr float k_maxRatioBetweenPointsOfInterest = 100.f;

  enum class PointOfInterest : uint8_t {
    None,
    Bound,
    Extremum,
    Root
  };

  /* TODO : These methods perform checks that will also be relevant for the
   * equation solver. Remember to factorize this code when integrating the new
   * solver. */
  static bool BoundOfIntervalOfDefinitionIsReached(float y1, float y2) { return std::isfinite(y1) && !std::isinf(y2) && std::isnan(y2); }
  static bool RootExistsOnInterval(float y1, float y2) { return ((y1 < 0.f && y2 > 0.f) || (y1 > 0.f && y2 < 0.f)); }
  static bool ExtremumExistsOnInterval(float y1, float y2, float y3) { return (y1 < y2 && y2 > y3) || (y1 > y2 && y2 < y3); }
  /* IsConvexAroundExtremum checks whether an interval contains an extremum or
   * an asymptote, by recursively computing the slopes. In case of an extremum,
   * the slope should taper off toward the center. */
  static bool IsConvexAroundExtremum(ValueAtAbscissa evaluation, float x1, float x2, float x3, float y1, float y2, float y3, Context * context, const void * auxiliary, int iterations = 3);
  /* If the function is discontinuous between its points of interest, there
   * might be a lot of empty space in the middle of the screen. In that case,
   * we want to zoom out to see more of the graph. */
  static void ExpandSparseWindow(float * sample, int length, float * xMin, float * xMax, float * yMin, float * yMax);
};

}

#endif
