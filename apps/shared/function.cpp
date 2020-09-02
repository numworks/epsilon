#include "function.h"
#include "range_1D.h"
#include "poincare_helpers.h"
#include "poincare/src/parsing/parser.h"
#include <ion/display.h>
#include <ion/unicode/utf8_decoder.h>
#include <algorithm>
#include <assert.h>
#include <cmath>
#include <float.h>
#include <string.h>

using namespace Poincare;

namespace Shared {

Function::NameNotCompliantError Function::BaseNameCompliant(const char * baseName) {
  assert(baseName[0] != 0);

  UTF8Decoder decoder(baseName);
  CodePoint c = decoder.nextCodePoint();
  if (c.isDecimalDigit()) {
    return NameNotCompliantError::NameCannotStartWithNumber;
  }

  while (c != UCodePointNull) {
    // FIXME '_' should be accepted but not as first character
    // TODO Factor this piece of code with similar one in the Parser
    if (!(c.isDecimalDigit() || c.isLatinLetter()) || c == '_') {
      return NameNotCompliantError::CharacterNotAllowed;
    }
    c = decoder.nextCodePoint();
  }

  if (Parser::IsReservedName(baseName, strlen(baseName))) {
    return NameNotCompliantError::ReservedName;
  }
  return NameNotCompliantError::None;
}

bool Function::isActive() const {
  return recordData()->isActive();
}

KDColor Function::color() const {
  return recordData()->color();
}

void Function::setActive(bool active) {
  recordData()->setActive(active);
}

int Function::printValue(double cursorT, double cursorX, double cursorY, char * buffer, int bufferSize, int precision, Poincare::Context * context) {
  return PoincareHelpers::ConvertFloatToText<double>(cursorY, buffer, bufferSize, precision);
}

int Function::name(char * buffer, size_t bufferSize) {
  return SymbolAbstract::TruncateExtension(buffer, fullName(), bufferSize);
}

int Function::nameWithArgument(char * buffer, size_t bufferSize) {
  int funcNameSize = name(buffer, bufferSize);
  assert(funcNameSize > 0);
  size_t result = funcNameSize;
  assert(result <= bufferSize);
  buffer[result++] = '(';
  assert(result <= bufferSize);
  assert(UTF8Decoder::CharSizeOfCodePoint(symbol()) <= 2);
  result += UTF8Decoder::CodePointToChars(symbol(), buffer+result, bufferSize-result);
  assert(result <= bufferSize);
  result += strlcpy(buffer+result, ")", bufferSize-result);
  return result;
}

Function::RecordDataBuffer * Function::recordData() const {
  assert(!isNull());
  Ion::Storage::Record::Data d = value();
  return reinterpret_cast<RecordDataBuffer *>(const_cast<void *>(d.buffer));
}

// Ranges
static float evaluateAndRound(const Function * f, float x, Context * context, float precision = 1e-5) {
  /* When evaluating sin(x)/x close to zero using the standard sine function,
   * one can detect small varitions, while the cardinal sine is supposed to be
   * locally monotonous. To smooth our such variations, we round the result of
   * the evaluations. As we are not interested in precise results but only in
   * ordering, this approximation is sufficient. */
  return precision * std::round(f->evaluateXYAtParameter(x, context).x2() / precision);
}

/* TODO : These three methods perform checks that will also be relevant for the
 * equation solver. Remember to factorize this code when integrating the new
 * solver. */
static bool boundOfIntervalOfDefinitionIsReached(float y1, float y2) {
  return std::isfinite(y1) && !std::isinf(y2) && std::isnan(y2);
}
static bool rootExistsOnInterval(float y1, float y2) {
  return ((y1 < 0.f && y2 > 0.f) || (y1 > 0.f && y2 < 0.f));
}
static bool extremumExistsOnInterval(float y1, float y2, float y3) {
  return (y1 < y2 && y2 > y3) || (y1 > y2 && y2 < y3);
}

/* This function checks whether an interval contains an extremum or an
 * asymptote, by recursively computing the slopes. In case of an extremum, the
 * slope should taper off toward the center. */
static bool isExtremum(const Function * f, float x1, float x2, float x3, float y1, float y2, float y3, Context * context, int iterations = 3) {
  if (iterations <= 0) {
    return false;
  }
  float x[2] = {x1, x3}, y[2] = {y1, y3};
  float xm, ym;
  for (int i = 0; i < 2; i++) {
    xm = (x[i] + x2) / 2.f;
    ym = evaluateAndRound(f, xm, context);
    bool res = ((y[i] < ym) != (ym < y2)) ? isExtremum(f, x[i], xm, x2, y[i], ym, y2, context, iterations - 1) : std::fabs(ym - y[i]) >= std::fabs(y2 - ym);
    if (!res) {
      return false;
    }
  }
  return true;
}

enum class PointOfInterest : uint8_t {
  None,
  Bound,
  Extremum,
  Root
};

void Function::rangeForDisplay(float * xMin, float * xMax, float * yMin, float * yMax, Context * context, bool tuneXRange) const {
  assert(xMin && xMax && yMin && yMax);

  /* Constants of the algorithm. */
  constexpr float defaultMaxInterval = 2e5f;
  constexpr float minDistance = 1e-2f;
  constexpr float asymptoteThreshold = 2e-1f;
  constexpr float stepFactor = 1.1f;
  constexpr int maxNumberOfPoints = 3;
  constexpr float breathingRoom = 0.3f;
  constexpr float maxRatioBetweenPoints = 100.f;

  const bool hasIntervalOfDefinition = std::isfinite(tMin()) && std::isfinite(tMax());
  float center, maxDistance;
  if (!tuneXRange) {
    center = (*xMax + *xMin) / 2.f;
    maxDistance = (*xMax - *xMin) / 2.f;
  } else if (hasIntervalOfDefinition) {
    center = (tMax() + tMin()) / 2.f;
    maxDistance = (tMax() - tMin()) / 2.f;
  } else {
    center = 0.f;
    maxDistance = defaultMaxInterval / 2.f;
  }

  float resultX[2] = {FLT_MAX, - FLT_MAX};
  float resultYMin = FLT_MAX, resultYMax = - FLT_MAX;
  float asymptote[2] = {FLT_MAX, - FLT_MAX};
  int numberOfPoints;
  float xFallback, yFallback[2] = {NAN, NAN};
  float firstResult;
  float dXOld, dXPrev, dXNext, yOld, yPrev, yNext;

  /* Look for a point of interest at the center. */
  const float a = center - minDistance - FLT_EPSILON, b = center + FLT_EPSILON, c = center + minDistance + FLT_EPSILON;
  const float ya = evaluateAndRound(this, a, context), yb = evaluateAndRound(this, b, context), yc = evaluateAndRound(this, c, context);
  if (boundOfIntervalOfDefinitionIsReached(ya, yc) ||
      boundOfIntervalOfDefinitionIsReached(yc, ya) ||
      rootExistsOnInterval(ya, yc) ||
      extremumExistsOnInterval(ya, yb, yc) || ya == yc)
  {
    resultX[0] = resultX[1] = center;
    if (extremumExistsOnInterval(ya, yb, yc) && isExtremum(this, a, b, c, ya, yb, yc, context)) {
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
    dXPrev = i == 0 ? - minDistance : minDistance;
    dXNext = dXPrev * stepFactor;
    yPrev = evaluateAndRound(this, center + dXPrev, context);
    yNext = evaluateAndRound(this, center + dXNext, context);

    while(std::fabs(dXPrev) < maxDistance) {
      /* Update the slider. */
      dXOld = dXPrev;
      dXPrev = dXNext;
      dXNext *= stepFactor;
      yOld = yPrev;
      yPrev = yNext;
      yNext = evaluateAndRound(this, center + dXNext, context);
      if (std::isinf(yNext)) {
        continue;
      }
      /* Check for a change in the profile. */
      const PointOfInterest variation = boundOfIntervalOfDefinitionIsReached(yPrev, yNext) ? PointOfInterest::Bound :
                            rootExistsOnInterval(yPrev, yNext) ? PointOfInterest::Root :
                            extremumExistsOnInterval(yOld, yPrev, yNext) ? PointOfInterest::Extremum :
                            PointOfInterest::None;
      switch (static_cast<uint8_t>(variation)) {
        /* The fallthrough is intentional, as we only want to update the Y
         * range when an extremum is detected, but need to update the X range
         * in all cases. */
        case static_cast<uint8_t>(PointOfInterest::Extremum):
        if (isExtremum(this, center + dXOld, center + dXPrev, center + dXNext, yOld, yPrev, yNext, context)) {
          resultYMin = std::min(resultYMin, yPrev);
          resultYMax = std::max(resultYMax, yPrev);
        }
      case static_cast<uint8_t>(PointOfInterest::Bound):
        /* We only count extrema / discontinuities for limiting the number
         * of points. This prevents cos(x) and cos(x)+2 from having different
         * profiles. */
        if (++numberOfPoints == maxNumberOfPoints) {
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
        if ((std::fabs(slopeNext) < asymptoteThreshold) && (std::fabs(slopePrev) > asymptoteThreshold)) {
          // Horizontal asymptote begins
          asymptote[i] = (i == 0) ? std::min(asymptote[i], center + dXNext) : std::max(asymptote[i], center + dXNext);
        } else if ((std::fabs(slopeNext) < asymptoteThreshold) && (std::fabs(slopePrev) > asymptoteThreshold)) {
          // Horizontal asymptote invalidates : it might be an asymptote when
          // going the other way.
          asymptote[1 - i] = (i == 1) ? std::min(asymptote[1 - i], center + dXPrev) : std::max(asymptote[1 - i], center + dXPrev);
        }
      }
    }
    if (std::fabs(resultX[i]) > std::fabs(firstResult) * maxRatioBetweenPoints && !std::isnan(xFallback)) {
      /* When there are too many points, cut them if their orders are too
       * different. */
      resultX[i] = xFallback;
      resultYMin = yFallback[0];
      resultYMax = yFallback[1];
    }
  }

  if (tuneXRange) {
    /* Cut after horizontal asymptotes. */
    resultX[0] = std::min(resultX[0], asymptote[0]);
    resultX[1] = std::max(resultX[1], asymptote[1]);
    if (resultX[0] >= resultX[1]) {
      /* Fallback to default range. */
      resultX[0] = - Range1D::k_default;
      resultX[1] = Range1D::k_default;
    } else {
      /* Add breathing room around points of interest. */
      float xRange = resultX[1] - resultX[0];
      resultX[0] -= breathingRoom * xRange;
      resultX[1] += breathingRoom * xRange;
      /* Round to the next integer. */
      resultX[0] = std::floor(resultX[0]);
      resultX[1] = std::ceil(resultX[1]);
    }
    *xMin = std::min(resultX[0], *xMin);
    *xMax = std::max(resultX[1], *xMax);
  }
  *yMin = std::min(resultYMin, *yMin);
  *yMax = std::max(resultYMax, *yMax);

  refinedYRangeForDisplay(*xMin, *xMax, yMin, yMax, context);
}

void Function::protectedRefinedYRangeForDisplay(float xMin, float xMax, float * yMin, float * yMax, Context * context, bool boundByMagnitude) const {
  /* This methods computes the Y range that will be displayed for cartesian
   * functions and sequences, given an X range (xMin, xMax) and bounds yMin and
   * yMax that must be inside the Y range.*/
  assert(yMin && yMax);

  constexpr int sampleSize = Ion::Display::Width / 4;
  constexpr float boundNegligigbleThreshold = 0.2f;

  float sampleYMin = FLT_MAX, sampleYMax = -FLT_MAX;
  const float step = (xMax - xMin) / (sampleSize - 1);
  float x, y;
  float sum = 0.f;
  int pop = 0;

  for (int i = 1; i < sampleSize; i++) {
    x = xMin + i * step;
    y = evaluateXYAtParameter(x, context).x2();
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
  if (*yMin > 0.f && *yMin / *yMax < boundNegligigbleThreshold) {
    *yMin = 0.f;
  } else if (*yMax < 0.f && *yMax / *yMin < boundNegligigbleThreshold) {
    *yMax = 0.f;
  }
}

}
