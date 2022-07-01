#ifndef POINCARE_ZOOM_H
#define POINCARE_ZOOM_H

#include <poincare/solver.h>
#include <ion/display.h>

/* FIXME : This class is concerned with manipulating the ranges of graphing
 * window, often represented by four float xMin, xMax, yMin, yMax. Handling
 * those same four values has proven repetititve, tredious, and prone to error.
 * This code could benefit from a data structure to regroup those values in a
 * single object. */

namespace Poincare {

class Zoom {
public:
  constexpr static float k_defaultHalfRange = 10.f;
  constexpr static float k_smallUnitMantissa = 1.f;
  constexpr static float k_mediumUnitMantissa = 2.f;
  constexpr static float k_largeUnitMantissa = 5.f;
  constexpr static float k_minimalRangeLength = 1e-4f;

  typedef SolverHelper<float>::ValueAtAbscissa ValueAtAbscissa;

  /* The following convention is taken for returned ranges :
   * - min < max : the range is valid.
   * - min = max : the range is a single point.
   * - min > max : the range is empty, min = FLT_MAX and max = -FLT_MAX are the
   *   only values allowed.  */

  /* Add breathing room around ranges. */
  static void AddBreathingRoom(float * xMin, float * xMax);
  /* Find the most suitable window to display the function's points of
   * interest.. */
  static void InterestingRangesForDisplay(ValueAtAbscissa evaluation, float * xMin, float * xMax, float * yMin, float * yMax, float tMin, float tMax, Context * context, const void * auxiliary);
  /* Find the best Y range to display the function on [xMin, xMax], but crop
   * the values that are outside of the function's order of magnitude. */
  static void RefinedYRangeForDisplay(ValueAtAbscissa evaluation, float xMin, float xMax, float * yMin, float * yMax, Context * context, const void * auxiliary);
  static void FullRange(ValueAtAbscissa evaluation, float tMin, float tMax, float tStep, float * fMin, float * fMax, Context * context, const void * auxiliary);
  static void RangeWithRatioForDisplay(ValueAtAbscissa evaluation, float yxRatio, float xMin, float xMax, float yMinForced, float yMaxForced, float * yMin, float * yMax, Context * context, const void * auxiliary);
  static void ExpandSparseWindow(ValueAtAbscissa evaluation, float * xMin, float * xMax, float * yMin, float * yMax, Context * context, const void * auxiliary);

  /* Find the bounding box of two given ranges. */
  static void CombineRanges(float min1, float max1, float min2, float max2, float * minRes, float * maxRes);
  /* Ensures that the range given as argument is not empty. */
  static void SanitizeRangeForDisplay(float * min, float * max, float defaultHalfWidth = k_defaultHalfRange);

  /* If shrink is false, the range will be set to ratio by increasing the size
   * of the smallest axis. If it is true, the longest axis will be reduced.*/
  static void SetToRatio(float yxRatio, float * xMin, float * xMax, float * yMin, float * yMax, bool shrink = false);
  static void SetZoom(float ratio, float xCenter, float yCenter, float * xMin, float * xMax, float * yMin, float * yMax);

private:
  constexpr static int k_peakNumberOfPointsOfInterest = 6;
  constexpr static int k_sampleSize = Ion::Display::Width / 4;
  constexpr static float k_maximalDistance = 1e5f;
  constexpr static float k_minimalDistance = 1e-2f;
  constexpr static float k_asymptoteThreshold = 2e-1f;
  constexpr static float k_explosionThreshold = 1e1f;
  constexpr static float k_stepFactor = 1.09f;
  constexpr static float k_breathingRoom = 0.3f;
  constexpr static float k_forceXAxisThreshold = 0.2f;
  constexpr static float k_maxRatioBetweenPointsOfInterest = 100.f;
  /* Chosen so that the overflow of e^x around x=100 would not overshadow
   * points in [-10,10]. */
  constexpr static float k_maxRatioBetweenBoundsAndPointsOfInterest = 10.f;

  enum class PointOfInterest : uint8_t {
    None,
    Bound,
    Extremum,
    Root
  };

  static bool BoundOfIntervalOfDefinitionIsReached(float y1, float y2) { return !std::isinf(y1) && !std::isinf(y2) && std::isnan(y1) != std::isnan(y2); }
  /* IsConvexAroundExtremum checks whether an interval contains an extremum or
   * an asymptote, by recursively computing the slopes. In case of an extremum,
   * the slope should taper off toward the center. */
  static bool IsConvexAroundExtremum(ValueAtAbscissa evaluation, float x1, float x2, float x3, float y1, float y2, float y3, Context * context, const void * auxiliary, int iterations = 7);
  static bool DoesNotOverestimatePrecision(float dx, float y1, float y2, float y3);
};

}

#endif
