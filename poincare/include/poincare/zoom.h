#ifndef POINCARE_ZOOM_H
#define POINCARE_ZOOM_H

#include <poincare/context.h>
#include <ion/display.h>

namespace Poincare {

class Zoom {
public:
  static constexpr float k_defaultHalfRange = 10.f;

  typedef float (*ValueAtAbscissa)(float abscissa, Context * context, const void * auxiliary);

  static void InterestingRangesForDisplay(ValueAtAbscissa evaluation, float * xMin, float * xMax, float * yMin, float * yMax, float tMin, float tMax, Context * context, const void * auxiliary, bool tuneXRange = false);
  static void RefinedYRangeForDisplay(ValueAtAbscissa evaluation, float xMin, float xMax, float * yMin, float * yMax, Context * context, const void * auxiliary, bool boundByMagnitude = false);

private:
  static constexpr int k_peakNumberOfPointsOfInterest = 3;
  static constexpr int k_sampleSize = Ion::Display::Width / 4;
  static constexpr float k_defaultMaxInterval = 2e5f;
  static constexpr float k_minimalDistance = 1e-2f;
  static constexpr float k_asymptoteThreshold = 2e-1f;
  static constexpr float k_stepFactor = 1.1f;
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
};

}

#endif
