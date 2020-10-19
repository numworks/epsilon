#include <poincare/zoom.h>
#include <float.h>
#include <algorithm>

namespace Poincare {

constexpr int
  Zoom::k_peakNumberOfPointsOfInterest,
  Zoom::k_sampleSize;
constexpr float
  Zoom::k_maximalDistance,
  Zoom::k_minimalDistance,
  Zoom::k_asymptoteThreshold,
  Zoom::k_explosionThreshold,
  Zoom::k_stepFactor,
  Zoom::k_breathingRoom,
  Zoom::k_forceXAxisThreshold,
  Zoom::k_defaultHalfRange,
  Zoom::k_maxRatioBetweenPointsOfInterest,
  Zoom::k_smallUnitMantissa,
  Zoom::k_mediumUnitMantissa,
  Zoom::k_largeUnitMantissa,
  Zoom::k_minimalRangeLength;

bool Zoom::InterestingRangesForDisplay(ValueAtAbscissa evaluation, float * xMin, float * xMax, float * yMin, float * yMax, float tMin, float tMax, Context * context, const void * auxiliary) {
  assert(xMin && xMax && yMin && yMax);

  const bool hasIntervalOfDefinition = std::isfinite(tMin) && std::isfinite(tMax);
  float center, maxDistance;
  if (hasIntervalOfDefinition) {
    center = (tMax + tMin) / 2.f;
    maxDistance = (tMax - tMin) / 2.f;
  } else {
    center = 0.f;
    maxDistance = k_maximalDistance;
  }

  float resultX[2] = {FLT_MAX, - FLT_MAX};
  float resultYMin = FLT_MAX, resultYMax = - FLT_MAX;
  float asymptote[2] = {FLT_MAX, - FLT_MAX};
  float explosion[2] = {FLT_MAX, - FLT_MAX};
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
        explosion[i] = i == 0 ? FLT_MAX : - FLT_MAX;
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
        } else if ((std::fabs(slopeNext) > k_asymptoteThreshold) && (std::fabs(slopePrev) < k_asymptoteThreshold)) {
          // Horizontal asymptote invalidates : it might be an asymptote when
          // going the other way.
          asymptote[1 - i] = (i == 1) ? std::min(asymptote[1 - i], center + dXPrev) : std::max(asymptote[1 - i], center + dXPrev);
        }
        if (std::fabs(slopeNext) > k_explosionThreshold && std::fabs(slopePrev) < k_explosionThreshold) {
          explosion[i] = (i == 0) ? std::min(explosion[i], center + dXNext) : std::max(explosion[i], center + dXNext);
        } else if (std::fabs(slopeNext) < k_explosionThreshold && std::fabs(slopePrev) > k_explosionThreshold) {
          explosion[1 - i] = (i == 1) ? std::min(explosion[1 - i], center + dXPrev) : std::max(explosion[1 - i], center + dXPrev);
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
    *xMin = NAN;
    *xMax = NAN;
    *yMin = NAN;
    *yMax = NAN;
    return false;
  } else {
    float xMinWithExplosion = std::min(resultX[0], explosion[0]);
    float xMaxWithExplosion = std::max(resultX[1], explosion[1]);
    if (xMaxWithExplosion - xMinWithExplosion < k_maxRatioBetweenPointsOfInterest * (resultX[1] - resultX[0])) {
      resultX[0] = xMinWithExplosion;
      resultX[1] = xMaxWithExplosion;
    }
    /* Add breathing room around points of interest. */
    float xRange = resultX[1] - resultX[0];
    resultX[0] -= k_breathingRoom * xRange;
    resultX[1] += k_breathingRoom * xRange;
  }
  *xMin = resultX[0];
  *xMax = resultX[1];
  *yMin = resultYMin;
  *yMax = resultYMax;

  return true;
}

void Zoom::RefinedYRangeForDisplay(ValueAtAbscissa evaluation, float xMin, float xMax, float * yMin, float * yMax, Context * context, const void * auxiliary, int sampleSize) {
  /* This methods computes the Y range that will be displayed for cartesian
   * functions and sequences, given an X range (xMin, xMax) and bounds yMin and
   * yMax that must be inside the Y range.*/
  assert(yMin && yMax);

  float sampleYMin = FLT_MAX, sampleYMax = -FLT_MAX;
  const float step = (xMax - xMin) / (sampleSize - 1);
  float x, y;
  float sum = 0.f;
  int pop = 0;

  for (int i = 1; i < sampleSize; i++) {
    x = xMin + i * step;
    y = evaluation(x, context, auxiliary);
    if (!std::isfinite(y)) {
      continue;
    }
    sampleYMin = std::min(sampleYMin, y);
    sampleYMax = std::max(sampleYMax, y);
    if (y != 0.f) {
      sum += std::log(std::fabs(y));
      pop++;
    }
  }
  /* sum/pop is the log mean value of the function, which can be interpreted as
   * its average order of magnitude. Then, bound is the value for the next
   * order of magnitude and is used to cut the Y range. */

  if (pop == 0) {
    *yMin = NAN;
    *yMax = NAN;
    return;
  } else {
    float bound = std::exp(sum / pop + 1.f);
    sampleYMin = std::max(sampleYMin, - bound);
    sampleYMax = std::min(sampleYMax, bound);
    *yMin = sampleYMin;
    *yMax = sampleYMax;
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

void Zoom::RangeWithRatioForDisplay(ValueAtAbscissa evaluation, float yxRatio, float * xMin, float * xMax, float * yMin, float * yMax, Context * context, const void * auxiliary) {
  constexpr float units[] = {k_smallUnitMantissa, k_mediumUnitMantissa, k_largeUnitMantissa};
  constexpr float rangeMagnitudeWeight = 0.2f;
  constexpr float maxMagnitudeDifference = 2.f;

  /* RefinedYRange for display, by default, evaluates the function 80 times,
   * and we call it 21 times. As we only need a rough estimate of the range to
   * compare it to the others, we save some time by only sampling the function
   * 20 times. */
  constexpr int sampleSize = k_sampleSize / 4;
  float bestGrade = FLT_MAX;
  float xMagnitude = k_minimalDistance;
  float yMinRange = FLT_MAX, yMaxRange = -FLT_MAX;
  while (xMagnitude < k_maximalDistance) {
    for(const float unit : units) {
      const float xRange = unit * xMagnitude;
      RefinedYRangeForDisplay(evaluation, -xRange, xRange, &yMinRange, &yMaxRange, context, auxiliary, sampleSize);
      float currentRatio = (yMaxRange - yMinRange) / (2 * xRange);
      float grade = std::fabs(std::log(currentRatio / yxRatio));
      /* When in doubt, favor ranges between [-1, 1] and [-10, 10] */
      grade += std::fabs(std::log(xRange / 10.f) * std::log(xRange)) * rangeMagnitudeWeight;
      if (std::fabs(std::log(currentRatio / yxRatio)) < maxMagnitudeDifference && grade < bestGrade) {
        bestGrade = grade;
        *xMin = -xRange;
        *xMax = xRange;
        *yMin = yMinRange;
        *yMax = yMaxRange;
      }
    }
    xMagnitude *= 10.f;
  }
  if (bestGrade == FLT_MAX) {
    *xMin = NAN;
    *xMax = NAN;
    *yMin = NAN;
    *yMax = NAN;
    return;
  }

  SetToRatio(yxRatio, xMin, xMax, yMin, yMax);
}

void Zoom::FullRange(ValueAtAbscissa evaluation, float tMin, float tMax, float tStep, float * fMin, float * fMax, Context * context, const void * auxiliary) {
  float t = tMin;
  *fMin = FLT_MAX;
  *fMax = -FLT_MAX;
  while (t <= tMax) {
    float value = evaluation(t, context, auxiliary);
    if (std::isfinite(value)) {
      *fMin = std::min(*fMin, value);
      *fMax = std::max(*fMax, value);
    }
    t += tStep;
  }
  if (*fMin > *fMax) {
    *fMin = NAN;
    *fMax = NAN;
  }
}

void Zoom::CombineRanges(int length, const float * mins, const float * maxs, float * minRes, float * maxRes) {
  ValueAtAbscissa evaluation = [](float x, Context * context, const void * auxiliary) {
    int index = std::round(x);
    return static_cast<const float *>(auxiliary)[index];
  };
  FullRange(evaluation, 0, length - 1, 1, minRes, maxRes, nullptr, mins);
  float min, max;
  FullRange(evaluation, 0, length - 1, 1, &min, &max, nullptr, maxs);
  if (std::isfinite(min)) {
    *minRes = std::min(min, *minRes);
  }
  if (std::isfinite(max)) {
    *maxRes = std::max(max, *maxRes);
  }
}

void Zoom::SanitizeRange(float * xMin, float * xMax, float * yMin, float * yMax, float normalRatio) {
  /* Axes of the window can be :
   *   - well-formed
   *   - empty (min = max)
   *   - ill-formed (min > max, or either bound is not finite)
   *
   * The general strategy to sanitize a window is as follow :
   *   - for all ill-formed axes, set both bounds to 0
   *   - if both axes are empty, set the X axis to default bounds
   *   - if one axis is empty, normalize the window
   *   - do nothing if both axes are well-formed. */

  if (!std::isfinite(*xMin) || !std::isfinite(*xMax) || *xMax < *xMin) {
    *xMin = 0;
    *xMax = 0;
  }
  if (!std::isfinite(*yMin) || !std::isfinite(*yMax) || *yMax < *yMin) {
    *yMin = 0;
    *yMax = 0;
  }

  float xRange = *xMax - *xMin;
  float yRange = *yMax - *yMin;
  if (xRange < k_minimalRangeLength && yRange < k_minimalRangeLength) {
    *xMax = *xMin + k_defaultHalfRange;
    *xMin -= k_defaultHalfRange;
    xRange = 2 * k_defaultHalfRange;
  }

  if (xRange < k_minimalRangeLength || yRange < k_minimalRangeLength) {
    SetToRatio(normalRatio, xMin, xMax, yMin, yMax, false);
  }
}

void Zoom::SetToRatio(float yxRatio, float * xMin, float * xMax, float * yMin, float * yMax, bool shrink) {
  float currentRatio = (*yMax - *yMin) / (*xMax - *xMin);

  float * tMin;
  float * tMax;
  float newRange;
  if ((currentRatio < yxRatio) == shrink) {
    /* Y axis too small and shrink, or Y axis too large and do not shrink
     * --> we change the X axis*/
    tMin = xMin;
    tMax = xMax;
    newRange = (*yMax - *yMin) / yxRatio;
  } else {
    tMin = yMin;
    tMax = yMax;
    newRange = (*xMax - *xMin) * yxRatio;
  }

  float center = (*tMax + *tMin) / 2.f;
  *tMax = center + newRange / 2.f;
  *tMin = center - newRange / 2.f;
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
