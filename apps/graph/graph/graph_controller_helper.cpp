#include "graph_controller_helper.h"
#include "../app.h"
#include <apps/shared/function_banner_delegate.h>
#include <apps/shared/poincare_helpers.h>
#include <poincare/ieee754.h>
#include <poincare/preferences.h>
#include <poincare/print.h>
#include <algorithm>

using namespace Shared;
using namespace Poincare;

namespace Graph {

bool GraphControllerHelper::privateMoveCursorHorizontally(Shared::CurveViewCursor * cursor, int direction, Shared::InteractiveCurveViewRange * range, int numberOfStepsInGradUnit, Ion::Storage::Record record, float pixelWidth, int scrollSpeed, int * subCurveIndex) {
  ExpiringPointer<ContinuousFunction> function = App::app()->functionStore()->modelForRecord(record);
  assert(!subCurveIndex || *subCurveIndex < function->numberOfSubCurves());
  const double tCursor = cursor->t();
  double tMin = function->tMin();
  double tMax = function->tMax();
  int functionsCount = -1;
  bannerView()->emptyInterestMessages(graphView()->cursorView());

  if (((direction > 0 && std::abs(tCursor-tMax) < DBL_EPSILON)
        || (direction < 0 && std::abs(tCursor-tMin) < DBL_EPSILON))
      && !App::app()->functionStore()->displaysNonCartesianFunctions(&functionsCount))
  {
    jumpToLeftRightCurve(tCursor, direction, functionsCount, record);
    return true;
  }
  Poincare::Context * context = App::app()->localContext();
  // Reload the expiring pointer
  function = App::app()->functionStore()->modelForRecord(record);
  double dir = (direction > 0 ? 1.0 : -1.0);

  bool specialConicCursorMove = false;
  if (function->properties().isConic() && function->numberOfSubCurves() == 2) {
    assert(subCurveIndex != nullptr);
    // previousXY will be needed for conic's special horizontal cursor moves.
    specialConicCursorMove = std::isfinite(function->evaluateXYAtParameter(tCursor, context, *subCurveIndex).x2());
    if (*subCurveIndex == 1 && !function->properties().isCartesianHyperbolaOfDegreeTwo()) {
      // On the sub curve, pressing left actually moves the cursor right
      dir *= -1.0;
    }
  }

  const float minimalAbsoluteStep = dir * pixelWidth;
  double step;
  float t = tCursor;
  if (function->properties().isCartesian()) {
    step = static_cast<double>(range->xGridUnit())/numberOfStepsInGradUnit;
    double slopeMultiplicator = 1.0;
    if (function->canDisplayDerivative()) {
      // Use the local derivative to slow down the cursor's step if needed
      double slope = function->approximateDerivative(tCursor, context, subCurveIndex ? *subCurveIndex : 0);
      if (std::isnan(slope)) {
        /* If the derivative could not bet computed, compute the derivative one
         * step further. */
        slope = function->approximateDerivative(tCursor + dir * step * pixelWidth, context, subCurveIndex ? *subCurveIndex : 0);
        if (std::isnan(slope)) {
          /* If the derivative is still NAN, it might mean that it's NAN
           * everywhere, so just set slope to a default value */
          slope = 1.0;
        }
      }
      // If yGridUnit is twice xGridUnit, visible slope is halved
      slope *= range->xGridUnit() / range->yGridUnit();
      /* Assuming the curve is a straight line of slope s. To move the cursor at
       * a fixed distance d along the line, the actual x-axis distance needed is
       * d' = d * cos(θ) with θ the angle between the line and the x-axis.
       * We also have tan(θ) = (s * d) / d = s
       * As a result, d' = d * cos(atan(s)) = d / sqrt(1 + s^2) */
      slopeMultiplicator /= std::sqrt(1.0 + slope * slope);
      // Add a sqrt(2) factor so that y=x isn't slowed down
      slopeMultiplicator *= std::sqrt(2.0);
    }

    // Prevent tStep from being too small before any snapping or rounding.
    float tStep = std::max(static_cast<float>(dir * step * slopeMultiplicator) * static_cast<float>(scrollSpeed), minimalAbsoluteStep);
    if (snapToInterestAndUpdateCursor(cursor, tCursor, tCursor + tStep * k_snapFactor, subCurveIndex ? *subCurveIndex : 0)) {
        // Cursor should have been updated by snapToInterest
        assert(tCursor != cursor->t());
        return true;
    }
    t += tStep;
    /* assert that it moved at least of 1 pixel.
     * round(t/pxWidth) is used by CurveView to compute the cursor's position. */
    if (std::fabs(static_cast<float>(tCursor)) >= pixelWidth && ((dir < 0) != (tCursor < 0)) && std::fabs(static_cast<float>(t)) < pixelWidth) {
      // Use a pixel width as a margin, ensuring t mostly stays at the same pixel
      // Round t to 0 if it is going into that direction, and is close enough
      t = 0.0;
    } else {
      // Round t to a simpler value, displayed at the same index
      double magnitude = std::pow(10.0, Poincare::IEEE754<double>::exponentBase10(pixelWidth));
      t = magnitude * std::round(t / magnitude);
      // Also round t so that f(x) matches f evaluated at displayed x
      t = FunctionBannerDelegate::GetValueDisplayedOnBanner(t, context, Preferences::sharedPreferences->numberOfSignificantDigits(), pixelWidth, false);
    }
    // Snap to interest could have corrupted ExpiringPointer
    function = App::app()->functionStore()->modelForRecord(record);
  } else {
    /* If function is not along X or Y, the cursor speed along t should not
     * depend on pixelWidth since the t interval can be very small even if the
     * pixel width is very large. */
    step = (tMax - tMin) / k_definitionDomainDivisor;
    t += dir * step * scrollSpeed;
    // If possible, round t so that f(x) matches f evaluated at displayed x
    t = FunctionBannerDelegate::GetValueDisplayedOnBanner(t, App::app()->localContext(), Preferences::sharedPreferences->numberOfSignificantDigits(), 0.05 * step, true);
  }
  // Ensure a minimal tStep again, allowing the crossing of asymptotes.
  if (t - tCursor < minimalAbsoluteStep) {
    t = tCursor + minimalAbsoluteStep;
  }
  // t must have changed
  assert(tCursor != t);

  t = std::max(tMin, std::min(tMax, static_cast<double>(t)));
  int subCurveIndexValue = subCurveIndex == nullptr ? 0 : *subCurveIndex;
  Coordinate2D<double> xy = function->evaluateXYAtParameter(t, context, subCurveIndexValue);

  if (specialConicCursorMove && std::isnan(xy.x2())) {
    if (function->properties().isCartesianHyperbolaOfDegreeTwo()) {
      // Hyperbolas have an undefined section along-side the x axis.
      double previousT = t;
      int tries = 0;
      int maxTries = std::ceil(numberOfStepsInGradUnit * Shared::CurveViewRange::k_maxNumberOfXGridUnits);
      do {
        // Try to jump out of the undefined section
        t += dir * step;
        xy = function->evaluateXYAtParameter(t, context, *subCurveIndex);
        tries ++;
      } while (std::isnan(xy.x2()) && tries < maxTries);
      if (tries >= maxTries || t < tMin || t > tMax) {
        // Reset to default t and xy
        t = previousT;
        xy = function->evaluateXYAtParameter(t, context, *subCurveIndex);
      }
    } else {
      /* The cursor would end up out of the conic's bounds, do not move the
       * cursor and switch to the other sub curve (with inverted dir) */
      t = tCursor;
      *subCurveIndex = 1 - *subCurveIndex;
      xy = function->evaluateXYAtParameter(t, context, *subCurveIndex);
    }
  }

  cursor->moveTo(t, xy.x1(), xy.x2());
  return true;
}

double GraphControllerHelper::reloadDerivativeInBannerViewForCursorOnFunction(Shared::CurveViewCursor * cursor, Ion::Storage::Record record) {
  ExpiringPointer<ContinuousFunction> function = App::app()->functionStore()->modelForRecord(record);
  double derivative = 0.0;

  // Force derivative to 0 if cursor is at an extremum
  PointsOfInterestCache * pointsOfInterest = App::app()->graphController()->pointsOfInterestForRecord(record);
  if (!pointsOfInterest->hasInterestAtCoordinates(cursor->x(), cursor->y(), Solver<double>::Interest::LocalMaximum) && !pointsOfInterest->hasInterestAtCoordinates(cursor->x(), cursor->y(), Solver<double>::Interest::LocalMinimum)) {
    derivative = function->approximateDerivative(cursor->x(), App::app()->localContext());
  }

  constexpr size_t bufferSize = FunctionBannerDelegate::k_textBufferSize;
  char buffer[bufferSize];
  int numberOfChar = function->derivativeNameWithArgument(buffer, bufferSize);
  assert(function->canDisplayDerivative());
  Poincare::Print::CustomPrintf(buffer + numberOfChar, bufferSize - numberOfChar, "=%*.*ed", derivative, Poincare::Preferences::sharedPreferences->displayMode(), Preferences::sharedPreferences->numberOfSignificantDigits());
  bannerView()->derivativeView()->setText(buffer);
  bannerView()->reload();

  return derivative;
}

bool GraphControllerHelper::snapToInterestAndUpdateCursor(Shared::CurveViewCursor * cursor, double start, double end, int subCurveIndex) {
  PointOfInterest nextPointOfInterest = App::app()->graphController()->pointsOfInterestForSelectedRecord()->firstPointInDirection(start, end, Poincare::Solver<double>::Interest::None, subCurveIndex);
  Coordinate2D<double> nextPointOfInterestXY = nextPointOfInterest.xy();
  if (!std::isfinite(nextPointOfInterestXY.x1())) {
    return false;
  }
  cursor->moveTo(nextPointOfInterest.abscissa(), nextPointOfInterestXY.x1(), nextPointOfInterestXY.x2());
  return true;
}

}
