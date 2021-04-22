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

void Zoom::InterestingRangesForDisplay(ValueAtAbscissa evaluation, float * xMin, float * xMax, float * yMin, float * yMax, float tMin, float tMax, Context * context, const void * auxiliary) {
  assert(xMin && xMax && yMin && yMax);

  float center, maxDistance;
  if (std::isfinite(tMin) && std::isfinite(tMax)) {
    center = (tMax + tMin) / 2.f;
    maxDistance = (tMax - tMin) / 2.f;
  } else {
    center = 0.f;
    maxDistance = k_maximalDistance;
  }

  SolverHelper<float>::BracketSearch search = [](float a, float b, float c, float fa, float fb, float fc, ValueAtAbscissa f, Context * context, const void * auxiliary) {
    if (BoundOfIntervalOfDefinitionIsReached(fa, fc)) {
      return Coordinate2D<float>(b, NAN);
    }
    if (SolverHelper<float>::RootExistsOnInterval(fa, fb, fc)) {
      return Coordinate2D<float>(b, 0.f);
    }
    if ((SolverHelper<float>::MinimumExistsOnInterval(fa, fb, fc) || SolverHelper<float>::MaximumExistsOnInterval(fa, fb, fc)) && DoesNotOverestimatePrecision(c, fa, fb, fc)) {
      return Coordinate2D<float>(
          b,
          IsConvexAroundExtremum(f, a, b, c, fa, fb, fc, context, auxiliary) ? fb : NAN);
    }
    /* Asymptotes and explosions
     * As the y value does not matter for an asymptote or explosion, it is used to notify the algorithm :
     *  - FLT_MIN (FLT_MAX) is the beginning of an asymptote (explosion)
     *  - -FLT_MIN (-FLT_MAX) is the end of an asymptote (explosion) */
    float slopeNext = std::fabs((fc - fb) / (c - b)), slopePrev = std::fabs((fb - fa) / (b - a));
    if (slopeNext < k_asymptoteThreshold && slopePrev > k_asymptoteThreshold) {
      return Coordinate2D<float>(b, FLT_MIN);
    } else if (slopeNext > k_asymptoteThreshold && slopePrev < k_asymptoteThreshold) {
      return Coordinate2D<float>(b, -FLT_MIN);
    }
    if (slopeNext > k_explosionThreshold && slopePrev < k_explosionThreshold) {
      return Coordinate2D<float>(b, FLT_MAX);
    } else if (slopeNext < k_explosionThreshold && slopePrev > k_explosionThreshold) {
      return Coordinate2D<float>(b, -FLT_MAX);
    }
    return Coordinate2D<float>(NAN, NAN);
  };

  float resultXMin = FLT_MAX, resultXMax = -FLT_MAX, resultYMin = FLT_MAX, resultYMax = -FLT_MAX;
  float fallbackXMin = NAN, fallbackXMax = NAN, fallbackYMin = NAN, fallbackYMax = NAN, firstPoint = NAN;
  float asymptoteXMin = FLT_MAX, asymptoteXMax = -FLT_MAX, explosionXMin = FLT_MAX, explosionXMax = -FLT_MAX;
  int numberOfExtrema = 0;

  /* Handle the center */
  float l = center - k_minimalDistance, r = center + k_minimalDistance;
  float fl = evaluation(l, context, auxiliary), fc = evaluation(center, context, auxiliary), fr = evaluation(r, context, auxiliary);
  if (std::isfinite(search(l, center, r, fl, fc, fr, evaluation, context, auxiliary).x1())
   || std::isfinite(search(r, center, l, fr, fc, fl, evaluation, context, auxiliary).x1())) {
    resultXMin = resultXMax = center;
    if (std::isfinite(fc)) {
      resultYMin = resultYMax = fc;
    }
  }

  float searchXLeft = l, searchXRight = r;
  int direction = 1;
  while (direction != 0) {
    Coordinate2D<float> pointOfInterest;
    if (direction > 0) {
      pointOfInterest = SolverHelper<float>::NextPointOfInterest(evaluation, context, auxiliary, search, searchXRight, center + maxDistance, k_stepFactor - 1.f, k_minimalDistance, FLT_MAX);
      searchXRight = pointOfInterest.x1();
    } else /* direction < 0 */ {
      pointOfInterest = SolverHelper<float>::NextPointOfInterest(evaluation, context, auxiliary, search, searchXLeft, center - maxDistance, k_stepFactor - 1.f, k_minimalDistance, FLT_MAX);
      searchXLeft = pointOfInterest.x1();
    }

    float x = pointOfInterest.x1(), y = pointOfInterest.x2();
    if (std::isnan(x)) {
      /* No point of interest left in this direction */
    } else if (std::fabs(y) == FLT_MAX) {
      if ((y < 0.f) == (direction < 0)) {
        explosionXMax = std::max(explosionXMax, x);
      } else {
        explosionXMin = std::min(explosionXMin, x);
      }
    } else if (std::fabs(y) == FLT_MIN) {
      if ((y < 0.f) == (direction < 0)) {
        asymptoteXMax = std::max(asymptoteXMax, x);
      } else {
        asymptoteXMin = std::min(asymptoteXMin, x);
      }
    } else {
      resultXMin = std::min(resultXMin, x);
      resultXMax = std::max(resultXMax, x);
      if (std::isnan(firstPoint)) {
        firstPoint = x;
      }
      if (y != 0.f && ++numberOfExtrema == k_peakNumberOfPointsOfInterest) {
        fallbackXMin = resultXMin;
        fallbackXMax = resultXMax;
        fallbackYMin = resultYMin;
        fallbackYMax = resultYMax;
      }
      if (std::isfinite(y)) {
        resultYMin = std::min(resultYMin, y);
        resultYMax = std::max(resultYMax, y);
      }
    }

    if (std::isfinite(searchXRight) && searchXRight < center + maxDistance) {
      if (std::isfinite(searchXLeft) && searchXLeft > center - maxDistance) {
        direction = (searchXRight - center) < (center - searchXLeft) ? 1 : -1;
      } else {
        direction = 1;
      }
    } else if (std::isfinite(searchXLeft) && searchXLeft > center - maxDistance) {
      direction = -1;
    } else {
      direction = 0;
    }
  }

  /* Combine the different boundaries */
  /* Asymptotes */
  resultXMin = std::min(resultXMin, asymptoteXMin);
  resultXMax = std::max(resultXMax, asymptoteXMax);
  /* Explosions, but only if it does not hide other points */
  float xMinWithExplosion = std::min(resultXMin, explosionXMin);
  float xMaxWithExplosion = std::max(resultXMax, explosionXMax);
  if (xMaxWithExplosion - xMinWithExplosion < k_maxRatioBetweenPointsOfInterest * (resultXMax - resultXMin)) {
    resultXMin = xMinWithExplosion;
    resultXMax = xMaxWithExplosion;
  }
  /* Cut the range if it is too large to show some points */
  if (resultXMax - resultXMin > std::fabs(firstPoint) * k_maxRatioBetweenPointsOfInterest && std::isfinite(fallbackXMin) && std::isfinite(fallbackXMax)) {
    resultXMin = fallbackXMin;
    resultXMax = fallbackXMax;
    resultYMin = fallbackYMin;
    resultYMax = fallbackYMax;
  }
  /* Add breathing room */
  if (resultXMin > resultXMax) {
    resultXMin = FLT_MAX;
    resultXMax = -FLT_MAX;
    assert(resultYMin == FLT_MAX && resultYMax == -FLT_MAX);
  } else {
    float xBreadth = resultXMin == resultXMax ? k_defaultHalfRange : k_breathingRoom * (resultXMax - resultXMin);
    resultXMin -= xBreadth;
    resultXMax += xBreadth;
  }
  *xMin = resultXMin;
  *xMax = resultXMax;
  *yMin = resultYMin;
  *yMax = resultYMax;
}

void Zoom::RefinedYRangeForDisplay(ValueAtAbscissa evaluation, float xMin, float xMax, float * yMin, float * yMax, Context * context, const void * auxiliary) {
  /* This methods computes the Y range that will be displayed for cartesian
   * functions and sequences, given an X range (xMin, xMax) and bounds yMin and
   * yMax that must be inside the Y range.*/
  assert(yMin && yMax);

  float sample[k_sampleSize];
  float sampleYMin = FLT_MAX, sampleYMax = -FLT_MAX;
  const float step = (xMax - xMin) / (k_sampleSize - 1);
  float x, y;
  float sum = 0.f;
  int pop = 0;

  sample[0] = evaluation(xMin, context, auxiliary);
  sample[k_sampleSize - 1] = evaluation(xMax, context, auxiliary);
  for (int i = 1; i < k_sampleSize - 1; i++) {
    x = xMin + i * step;
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
    *yMin = FLT_MAX;
    *yMax = -FLT_MAX;
    return;
  }
  float bound = std::exp(sum / pop + 1.f);
  *yMin = std::max(sampleYMin, - bound);
  *yMax = std::min(sampleYMax, bound);
  /* Round out the smallest bound to 0 if it is negligible compared to the
   * other one. This way, we can display the X axis for positive functions such
   * as sqrt(x) even if we do not sample close to 0. */
  if (*yMin > 0.f && *yMin / *yMax < k_forceXAxisThreshold) {
    *yMin = 0.f;
  } else if (*yMax < 0.f && *yMax / *yMin < k_forceXAxisThreshold) {
    *yMax = 0.f;
  }
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

void Zoom::CombineRanges(float min1, float max1, float min2, float max2, float * minRes, float * maxRes) {
  assert(std::isfinite(min1) && std::isfinite(max1) && std::isfinite(min2) && std::isfinite(max2));
  assert(minRes != nullptr && maxRes != nullptr);
  *minRes = std::min(min1, min2);
  *maxRes = std::max(max1, max2);
  assert(*minRes <= *maxRes || (*minRes == FLT_MAX && *maxRes == -FLT_MAX));
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

bool Zoom::DoesNotOverestimatePrecision(float dx, float y1, float y2, float y3) {
  /* The float type looses precision surprisingly fast, and cannot confidently
   * hold more than 6.6 digits of precision. Results more precise than that are
   * too noisy to be be of any value. */
  float yMin = std::min(y1, std::min(y2, y3));
  float yMax = std::max(y1, std::max(y2, y3));
  constexpr float maxPrecision = 2.f * FLT_EPSILON;
  return (yMax - yMin) / std::fabs(dx) > maxPrecision;
}

}
