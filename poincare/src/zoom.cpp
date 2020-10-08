#include <poincare/zoom.h>
#include <float.h>
#include <algorithm>

namespace Poincare {

constexpr int
  Zoom::k_peakNumberOfPointsOfInterest,
  Zoom::k_sampleSize;
constexpr float
  Zoom::k_defaultMaxInterval,
  Zoom::k_minimalDistance,
  Zoom::k_asymptoteThreshold,
  Zoom::k_stepFactor,
  Zoom::k_breathingRoom,
  Zoom::k_forceXAxisThreshold,
  Zoom::k_defaultHalfRange,
  Zoom::k_maxRatioBetweenPointsOfInterest;

void Zoom::InterestingRangesForDisplay(ValueAtAbscissa evaluation, float * xMin, float * xMax, float * yMin, float * yMax, float tMin, float tMax, Context * context, const void * auxiliary) {
  assert(xMin && xMax && yMin && yMax);

  const bool hasIntervalOfDefinition = std::isfinite(tMin) && std::isfinite(tMax);
  float center, maxDistance;
  if (hasIntervalOfDefinition) {
    center = (tMax + tMin) / 2.f;
    maxDistance = (tMax - tMin) / 2.f;
  } else {
    center = 0.f;
    maxDistance = k_defaultMaxInterval / 2.f;
  }

  float resultX[2] = {FLT_MAX, - FLT_MAX};
  float resultYMin = FLT_MAX, resultYMax = - FLT_MAX;
  float asymptote[2] = {FLT_MAX, - FLT_MAX};
  int numberOfPoints;
  float xFallback, yFallback[2] = {NAN, NAN};
  float firstResult;
  float dXOld, dXPrev, dXNext, yOld, yPrev, yNext;

  /* Look for a point of interest at the center. */
  const float a = center - k_minimalDistance - FLT_EPSILON, b = center + FLT_EPSILON, c = center + k_minimalDistance + FLT_EPSILON;
  const float ya = evaluation(a, context, auxiliary), yb = evaluation(b, context, auxiliary), yc = evaluation(c, context, auxiliary);
  if (BoundOfIntervalOfDefinitionIsReached(ya, yc) ||
      BoundOfIntervalOfDefinitionIsReached(yc, ya) ||
      RootExistsOnInterval(ya, yc) ||
      ExtremumExistsOnInterval(ya, yb, yc) || ya == yc)
  {
    resultX[0] = resultX[1] = center;
    if (ExtremumExistsOnInterval(ya, yb, yc) && IsConvexAroundExtremum(evaluation, a, b, c, ya, yb, yc, context, auxiliary)) {
      resultYMin = resultYMax = yb;
    }
  }

  /* We search for points of interest by exploring the function leftward from
   * the center and then rightward, hence the two iterations. */
  for (int i = 0; i < 2; i++) {
    /* Initialize the search parameters. */
    numberOfPoints = 0;
    firstResult = NAN;
    xFallback = NAN;
    dXPrev = i == 0 ? - k_minimalDistance : k_minimalDistance;
    dXNext = dXPrev * k_stepFactor;
    yPrev = evaluation(center + dXPrev, context, auxiliary);
    yNext = evaluation(center + dXNext, context, auxiliary);

    while(std::fabs(dXPrev) < maxDistance) {
      /* Update the slider. */
      dXOld = dXPrev;
      dXPrev = dXNext;
      dXNext *= k_stepFactor;
      yOld = yPrev;
      yPrev = yNext;
      yNext = evaluation(center + dXNext, context, auxiliary);
      if (std::isinf(yNext)) {
        continue;
      }
      /* Check for a change in the profile. */
      const PointOfInterest variation = BoundOfIntervalOfDefinitionIsReached(yPrev, yNext) ? PointOfInterest::Bound :
                            RootExistsOnInterval(yPrev, yNext) ? PointOfInterest::Root :
                            ExtremumExistsOnInterval(yOld, yPrev, yNext) ? PointOfInterest::Extremum :
                            PointOfInterest::None;
      switch (static_cast<uint8_t>(variation)) {
        /* The fallthrough is intentional, as we only want to update the Y
         * range when an extremum is detected, but need to update the X range
         * in all cases. */
      case static_cast<uint8_t>(PointOfInterest::Extremum):
        if (IsConvexAroundExtremum(evaluation, center + dXOld, center + dXPrev, center + dXNext, yOld, yPrev, yNext, context, auxiliary)) {
          resultYMin = std::min(resultYMin, yPrev);
          resultYMax = std::max(resultYMax, yPrev);
        }
      case static_cast<uint8_t>(PointOfInterest::Bound):
        /* We only count extrema / discontinuities for limiting the number
         * of points. This prevents cos(x) and cos(x)+2 from having different
         * profiles. */
        if (++numberOfPoints == k_peakNumberOfPointsOfInterest) {
          /* When too many points are encountered, we prepare their erasure by
           * setting a restore point. */
          xFallback = dXNext + center;
          yFallback[0] = resultYMin;
          yFallback[1] = resultYMax;
        }
      case static_cast<uint8_t>(PointOfInterest::Root):
        asymptote[i] = i == 0 ? FLT_MAX : - FLT_MAX;
        resultX[0] = std::min(resultX[0], center + (i == 0 ? dXNext : dXPrev));
        resultX[1] = std::max(resultX[1], center + (i == 1 ? dXNext : dXPrev));
        if (std::isnan(firstResult)) {
          firstResult = dXNext;
        }
        break;
      default:
        const float slopeNext = (yNext - yPrev) / (dXNext - dXPrev), slopePrev = (yPrev - yOld) / (dXPrev - dXOld);
        if ((std::fabs(slopeNext) < k_asymptoteThreshold) && (std::fabs(slopePrev) > k_asymptoteThreshold)) {
          // Horizontal asymptote begins
          asymptote[i] = (i == 0) ? std::min(asymptote[i], center + dXNext) : std::max(asymptote[i], center + dXNext);
        } else if ((std::fabs(slopeNext) < k_asymptoteThreshold) && (std::fabs(slopePrev) > k_asymptoteThreshold)) {
          // Horizontal asymptote invalidates : it might be an asymptote when
          // going the other way.
          asymptote[1 - i] = (i == 1) ? std::min(asymptote[1 - i], center + dXPrev) : std::max(asymptote[1 - i], center + dXPrev);
        }
      }
    }
    if (std::fabs(resultX[i]) > std::fabs(firstResult) * k_maxRatioBetweenPointsOfInterest && !std::isnan(xFallback)) {
      /* When there are too many points, cut them if their orders are too
       * different. */
      resultX[i] = xFallback;
      resultYMin = yFallback[0];
      resultYMax = yFallback[1];
    }
  }

  /* Cut after horizontal asymptotes. */
  resultX[0] = std::min(resultX[0], asymptote[0]);
  resultX[1] = std::max(resultX[1], asymptote[1]);
  if (resultX[0] >= resultX[1]) {
    /* Fallback to default range. */
    resultX[0] = - k_defaultHalfRange;
    resultX[1] = k_defaultHalfRange;
  } else {
    /* Add breathing room around points of interest. */
    float xRange = resultX[1] - resultX[0];
    resultX[0] -= k_breathingRoom * xRange;
    resultX[1] += k_breathingRoom * xRange;
    /* Round to the next integer. */
    resultX[0] = std::floor(resultX[0]);
    resultX[1] = std::ceil(resultX[1]);
  }
  *xMin = std::min(resultX[0], *xMin);
  *xMax = std::max(resultX[1], *xMax);

  *yMin = std::min(resultYMin, *yMin);
  *yMax = std::max(resultYMax, *yMax);
}

void Zoom::RefinedYRangeForDisplay(ValueAtAbscissa evaluation, float xMin, float xMax, float * yMin, float * yMax, Context * context, const void * auxiliary, bool boundByMagnitude) {
  /* This methods computes the Y range that will be displayed for cartesian
   * functions and sequences, given an X range (xMin, xMax) and bounds yMin and
   * yMax that must be inside the Y range.*/
  assert(yMin && yMax);

  float sampleYMin = FLT_MAX, sampleYMax = -FLT_MAX;
  const float step = (xMax - xMin) / (k_sampleSize - 1);
  float x, y;
  float sum = 0.f;
  int pop = 0;

  for (int i = 1; i < k_sampleSize; i++) {
    x = xMin + i * step;
    y = evaluation(x, context, auxiliary);
    if (!std::isfinite(y)) {
      continue;
    }
    sampleYMin = std::min(sampleYMin, y);
    sampleYMax = std::max(sampleYMax, y);
    if (std::fabs(y) > FLT_EPSILON) {
      sum += std::log(std::fabs(y));
      pop++;
    }
  }
  /* sum/pop is the log mean value of the function, which can be interpreted as
   * its average order of magnitude. Then, bound is the value for the next
   * order of magnitude and is used to cut the Y range. */
  if (boundByMagnitude) {
    float bound = (pop > 0) ? std::exp(sum / pop + 1.f) : FLT_MAX;
    sampleYMin = std::max(sampleYMin, - bound);
    sampleYMax = std::min(sampleYMax, bound);
  }
  *yMin = std::min(*yMin, sampleYMin);
  *yMax = std::max(*yMax, sampleYMax);
  if (*yMin == *yMax) {
    float d = (*yMin == 0.f) ? 1.f : *yMin * 0.2f;
    *yMin -= d;
    *yMax += d;
  }
  /* Round out the smallest bound to 0 if it is negligible compare to the
   * other one. This way, we can display the X axis for positive functions such
   * as sqrt(x) even if we do not sample close to 0. */
  if (*yMin > 0.f && *yMin / *yMax < k_forceXAxisThreshold) {
    *yMin = 0.f;
  } else if (*yMax < 0.f && *yMax / *yMin < k_forceXAxisThreshold) {
    *yMax = 0.f;
  }
}

bool Zoom::IsConvexAroundExtremum(ValueAtAbscissa evaluation, float x1, float x2, float x3, float y1, float y2, float y3, Context * context, const void * auxiliary, int iterations) {
  if (iterations <= 0) {
    return false;
  }
  float x[2] = {x1, x3}, y[2] = {y1, y3};
  float xm, ym;
  for (int i = 0; i < 2; i++) {
    xm = (x[i] + x2) / 2.f;
    ym = evaluation(xm, context, auxiliary);
    bool res = ((y[i] < ym) != (ym < y2)) ? IsConvexAroundExtremum(evaluation, x[i], xm, x2, y[i], ym, y2, context, auxiliary, iterations - 1) : std::fabs(ym - y[i]) >= std::fabs(y2 - ym);
    if (!res) {
      return false;
    }
  }
  return true;
}

}
