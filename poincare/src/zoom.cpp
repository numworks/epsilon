#include <poincare/zoom.h>
#include <poincare/helpers.h>
#include <float.h>
#include <algorithm>
#include <stddef.h>

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

static bool DoesNotOverestimatePrecision(float dx, float y1, float y2, float y3) {
  /* The float type looses precision surprisingly fast, and cannot confidently
   * hold more than 6.6 digits of precision. Results more precise than that are
   * too noisy to be be of any value. */
  float yMin = std::min(y1, std::min(y2, y3));
  float yMax = std::max(y1, std::max(y2, y3));
  constexpr float maxPrecision = 2.f * FLT_EPSILON;
  return (yMax - yMin) / std::fabs(dx) > maxPrecision;
}

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
  int numberOfPoints, totalNumberOfPoints = 0;
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
    totalNumberOfPoints++;
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
                            (ExtremumExistsOnInterval(yOld, yPrev, yNext) && DoesNotOverestimatePrecision(dXNext, yOld, yPrev, yNext)) ? PointOfInterest::Extremum :
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
        totalNumberOfPoints++;
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

  if (totalNumberOfPoints == 1) {
    float xM = (resultX[0] + resultX[1]) / 2;
    resultX[0] = xM;
    resultX[1] = xM;
  }
  /* Cut after horizontal asymptotes. */
  resultX[0] = std::min(resultX[0], asymptote[0]);
  resultX[1] = std::max(resultX[1], asymptote[1]);
  /* Cut after explosions if it does not reduce precision */
  float xMinWithExplosion = std::min(resultX[0], explosion[0]);
  float xMaxWithExplosion = std::max(resultX[1], explosion[1]);
  if (xMaxWithExplosion - xMinWithExplosion < k_maxRatioBetweenPointsOfInterest * (resultX[1] - resultX[0])) {
    resultX[0] = xMinWithExplosion;
    resultX[1] = xMaxWithExplosion;
  }
  if (resultX[0] >= resultX[1]) {
    if (resultX[0] > resultX[1]) {
      resultX[0] = NAN;
      resultX[1] = NAN;
    }
    /* Fallback to default range. */
    *xMin = resultX[0];
    *xMax = resultX[1];
    *yMin = NAN;
    *yMax = NAN;
    return false;
  } else {
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

void Zoom::RefinedYRangeForDisplay(ValueAtAbscissa evaluation, float * xMin, float * xMax, float * yMin, float * yMax, Context * context, const void * auxiliary) {
  /* This methods computes the Y range that will be displayed for cartesian
   * functions and sequences, given an X range (xMin, xMax) and bounds yMin and
   * yMax that must be inside the Y range.*/
  assert(yMin && yMax);

  float sample[k_sampleSize];
  float sampleYMin = FLT_MAX, sampleYMax = -FLT_MAX;
  const float step = (*xMax - *xMin) / (k_sampleSize - 1);
  float x, y;
  float sum = 0.f;
  int pop = 0;

  sample[0] = evaluation(*xMin, context, auxiliary);
  sample[k_sampleSize - 1] = evaluation(*xMax, context, auxiliary);
  for (int i = 1; i < k_sampleSize - 1; i++) {
    x = *xMin + i * step;
    y = evaluation(x, context, auxiliary);
    sample[i] = y;
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
    *yMin = std::isfinite(*yMin) ? std::min(*yMin, sampleYMin) : sampleYMin;
    *yMax = std::isfinite(*yMax) ? std::max(*yMax, sampleYMax) : sampleYMax;
  }
  /* Round out the smallest bound to 0 if it is negligible compare to the
   * other one. This way, we can display the X axis for positive functions such
   * as sqrt(x) even if we do not sample close to 0. */
  if (*yMin > 0.f && *yMin / *yMax < k_forceXAxisThreshold) {
    *yMin = 0.f;
  } else if (*yMax < 0.f && *yMax / *yMin < k_forceXAxisThreshold) {
    *yMax = 0.f;
  }

  ExpandSparseWindow(sample, k_sampleSize, xMin, xMax, yMin, yMax);
}

void Zoom::RangeWithRatioForDisplay(ValueAtAbscissa evaluation, float yxRatio, float * xMin, float * xMax, float * yMin, float * yMax, Context * context, const void * auxiliary) {
  /* The goal of this algorithm is to find the window with given ratio, that
   * best suits the function.
   * - The X range is centered around a point of interest of the function, or
   *   0 if none exist, and uses a default width of 20.
   * - The Y range's height is fixed at 20*yxRatio. Its center is chosen to
   *   maximize the number of visible points of the function. */
  constexpr float minimalXCoverage = 0.15f;
  constexpr float minimalYCoverage = 0.3f;
  constexpr int sampleSize = k_sampleSize * 2;

  float xCenter = *xMin == *xMax ? *xMin : 0.f;
  *xMin = xCenter - k_defaultHalfRange;
  *xMax = xCenter + k_defaultHalfRange;
  float xRange = 2 * k_defaultHalfRange;
  float step = xRange / (sampleSize - 1);
  float sample[sampleSize];
  for (int i = 0; i < sampleSize; i++) {
    sample[i] = evaluation(*xMin + i * step, context, auxiliary);
  }
  Helpers::Sort(
      [](int i, int j, void * ctx, int size) {
        float * array = static_cast<float *>(ctx);
        float temp = array[i];
        array[i] = array[j];
        array[j] = temp;
      },
      [](int i, int j, void * ctx, int size) {
        float * array = static_cast<float *>(ctx);
        return array[i] >= array[j];
      },
      sample,
      sampleSize);

  /* For each value taken by the sample of the function on [xMin, xMax], given
   * a fixed value for yRange, we measure the number (referred to as breadth)
   * of other points that could be displayed if this value was chosen as yMin.
   * In other terms, given a sorted set Y={y1,...,yn} and a length dy, we look
   * for the pair 1<=i,j<=n such that :
   *   - yj - yi <= dy
   *   - i - j is maximal
   * The fact that the sample is sorted makes it possible to find i and j in
   * linear time.
   * In case of pairs having the same breadth, we chose the pair that minimizes
   * the criteria distanceFromCenter, which makes the window symmetrical when
   * dealing with linear functions. */
  float yRange = yxRatio * xRange;
  int j = 1;
  int bestIndex = 0, bestBreadth = 0, bestDistanceToCenter;
  for (int i = 0; i < sampleSize; i++) {
    if (sampleSize - i < bestBreadth) {
      break;
    }
    while (j < sampleSize && sample[j] < sample[i] + yRange) {
      j++;
    }
    int breadth = j - i;
    int distanceToCenter = std::fabs(static_cast<float>(i + j - sampleSize));
    if (breadth > bestBreadth
     || (breadth == bestBreadth
      && distanceToCenter <= bestDistanceToCenter)) {
      bestIndex = i;
      bestBreadth = breadth;
      bestDistanceToCenter = distanceToCenter;
    }
  }

  /* Functions with a very steep slope might only take a small portion of the
   * X axis. Conversely, very flat functions may only take a small portion of
   * the Y range. In those cases, the ratio is not suitable. */
  if (bestBreadth < minimalXCoverage * sampleSize
   || sample[bestIndex + bestBreadth - 1] - sample[bestIndex] < minimalYCoverage * yRange) {
    *xMin = NAN;
    *xMax = NAN;
    *yMin = NAN;
    *yMax = NAN;
    return;
  }

  float yCenter = (sample[bestIndex] + sample[bestIndex + bestBreadth - 1]) / 2.f;
  *yMin = yCenter - yRange / 2.f;
  *yMax = yCenter + yRange / 2.f;
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

void Zoom::SetZoom(float ratio, float xCenter, float yCenter, float * xMin, float * xMax, float * yMin, float * yMax) {
  float oneMinusRatio = 1.f - ratio;
  *xMin = oneMinusRatio * xCenter + ratio * *xMin;
  *xMax = oneMinusRatio * xCenter + ratio * *xMax;
  *yMin = oneMinusRatio * yCenter + ratio * *yMin;
  *yMax = oneMinusRatio * yCenter + ratio * *yMax;
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

void Zoom::ExpandSparseWindow(float * sample, int length, float * xMin, float * xMax, float * yMin, float * yMax) {
  /* We compute the "empty center" of the window, i.e. the largest rectangle
   * (with same center and shape as the window) that does not contain any
   * point. If that rectangle is deemed too large, we consider that not enough
   * of the curve shows up on screen and we zoom out. */
  constexpr float emptyCenterMaxSize = 0.5f;
  constexpr float ratioCorrection = 4.f/3.f;

  float xCenter = (*xMax + *xMin) / 2.f;
  float yCenter = (*yMax + *yMin) / 2.f;
  float xRange = *xMax - *xMin;
  float yRange = *yMax - *yMin;

  float emptyCenter = FLT_MAX;
  float step = xRange / (length - 1);
  for (int i = 0; i < length; i++) {
    float x = *xMin + i * step;
    float y = sample[i];
    if (std::isfinite(y)) {
      /* r is the ratio between the window and the largest rectangle (with same
       * center and shape as the window) that does not contain (x,y).
       * i.e. the smallest zoom-in for which (x,y) is not visible. */
      float r = 2 * std::max(std::fabs(x - xCenter) / xRange, std::fabs(y - yCenter) / yRange);
      emptyCenter = std::min(emptyCenter, r);
    }
  }

  if (emptyCenter > emptyCenterMaxSize) {
    SetZoom(ratioCorrection + emptyCenter, xCenter, yCenter, xMin, xMax, yMin ,yMax);
  }
}

}
