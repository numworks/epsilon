#include "graph_controller_helper.h"

#include <apps/shared/function_banner_delegate.h>
#include <apps/shared/poincare_helpers.h>
#include <omg/ieee754.h>
#include <poincare/old/point_evaluation.h>
#include <poincare/preferences.h>
#include <poincare/print.h>

#include <algorithm>

#include "../app.h"

using namespace Shared;
using namespace Poincare;

namespace Graph {

bool GraphControllerHelper::privateMoveCursorHorizontally(
    CurveViewCursor* cursor, OMG::HorizontalDirection direction,
    InteractiveCurveViewRange* range, int numberOfStepsInGradUnit,
    Ion::Storage::Record record, float pixelWidth, int scrollSpeed,
    int* subCurveIndex) {
  ExpiringPointer<ContinuousFunction> function =
      App::app()->functionStore()->modelForRecord(record);
  assert(!subCurveIndex || *subCurveIndex < function->numberOfSubCurves(true));
  const double tCursor = cursor->t();
  double tMin = function->tMin();
  double tMax = function->tMax();
  int functionsCount = -1;
  bannerView()->emptyInterestMessages(graphView()->cursorView());

  if (((direction.isRight() && std::abs(tCursor - tMax) < DBL_EPSILON) ||
       (direction.isLeft() && std::abs(tCursor - tMin) < DBL_EPSILON)) &&
      App::app()->functionStore()->displaysOnlyCartesianFunctions(
          &functionsCount)) {
    jumpToLeftRightCurve(tCursor, direction, functionsCount, record);
    return true;
  }
  Context* context = App::app()->localContext();
  // Reload the expiring pointer
  function = App::app()->functionStore()->modelForRecord(record);
  double dir = (direction.isRight() ? 1.0 : -1.0);

  bool specialConicCursorMove = false;
  if (function->properties().isConic() && function->numberOfSubCurves() == 2) {
    assert(subCurveIndex != nullptr);
    // previousXY will be needed for conic's special horizontal cursor moves.
    specialConicCursorMove = std::isfinite(
        function->evaluateXYAtParameter(tCursor, context, *subCurveIndex).y());
    if (*subCurveIndex == 1 &&
        !function->properties().isCartesianHyperbolaOfDegreeTwo()) {
      // On the sub curve, pressing left actually moves the cursor right
      dir *= -1.0;
    }
  }

  double step;
  double t = tCursor;
  if (function->properties().isCartesian()) {
    step = static_cast<double>(range->xGridUnit()) / numberOfStepsInGradUnit;
    double slopeMultiplicator = 1.0;
    if (function->canDisplayDerivative()) {
      // Use the local derivative to slow down the cursor's step if needed
      double slope =
          function->approximateDerivative<double>(tCursor, context).toScalar();
      if ((!subCurveIndex || *subCurveIndex == 0) && std::isnan(slope)) {
        /* If the derivative could not bet computed, compute the derivative one
         * step further. */
        slope = function
                    ->approximateDerivative<double>(
                        tCursor + dir * step * pixelWidth, context)
                    .toScalar();
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

    const double minimalAbsoluteStep = pixelWidth;
    /* Prevent tStep from being too small before any snapping or rounding.
     * A bigger minimal step is enforced so that t can be rounded down. */
    double tStep = dir * std::max(step * slopeMultiplicator *
                                      static_cast<double>(scrollSpeed),
                                  minimalAbsoluteStep * 2);
    if (snapToInterestAndUpdateCursor(cursor, tCursor,
                                      tCursor + tStep * k_snapFactor,
                                      subCurveIndex ? *subCurveIndex : 0)) {
      // Cursor should have been updated by snapToInterest
      assert(tCursor != cursor->t());
      return true;
    }
    t += tStep;
    /* assert that it moved at least of 1 pixel.
     * round(t/pxWidth) is used by CurveView to compute the cursor's position.
     */
    if (std::fabs(static_cast<float>(tCursor)) >= pixelWidth &&
        ((dir < 0.0) != (tCursor < 0.0)) &&
        std::fabs(static_cast<float>(t)) < pixelWidth) {
      /* Use a pixel width as a margin, ensuring t mostly stays at the same
       * pixel Round t to 0 if it is going into that direction, and is close
       * enough. */
      t = 0.0;
    } else {
      // Round t to a simpler value, displayed at the same index
      double magnitude =
          std::pow(10.0, OMG::IEEE754<double>::exponentBase10(pixelWidth));
      t = magnitude * std::round(t / magnitude);
      // Also round t so that f(x) matches f evaluated at displayed x
      t = FunctionBannerDelegate::GetValueDisplayedOnBanner(
          t, context,
          Preferences::SharedPreferences()->numberOfSignificantDigits(),
          pixelWidth, false);
    }
    // Snap to interest could have corrupted ExpiringPointer
    function = App::app()->functionStore()->modelForRecord(record);

    // Ensure a minimal tStep again, allowing the crossing of asymptotes.
    if (std::abs(t - tCursor) < minimalAbsoluteStep) {
      t = tCursor + dir * minimalAbsoluteStep;
    }
  } else if (function->properties().isScatterPlot()) {
    // Silence warning for step being potentially uninitialized.
    step = 1.0;
    float newT = std::floor(t + dir);
    if (0.f <= newT && newT < function->iterateScatterPlot(context).length()) {
      t = newT;
    }
  } else {
    /* If function is not along X or Y, the cursor speed along t should not
     * depend on pixelWidth since the t interval can be very small even if the
     * pixel width is very large. */
    step = (tMax - tMin) / k_definitionDomainDivisor;
    t += dir * step * scrollSpeed;
    // If possible, round t so that f(x) matches f evaluated at displayed x
    t = FunctionBannerDelegate::GetValueDisplayedOnBanner(
        t, App::app()->localContext(),
        Preferences::SharedPreferences()->numberOfSignificantDigits(),
        0.05 * step, true);
  }
  // t must have changed
  assert(tCursor != t || function->properties().isScatterPlot());

  t = std::max(tMin, std::min(tMax, t));
  int subCurveIndexValue = subCurveIndex == nullptr ? 0 : *subCurveIndex;
  Coordinate2D<double> xy =
      function->evaluateXYAtParameter(t, context, subCurveIndexValue);

  if (specialConicCursorMove && std::isnan(xy.y())) {
    if (function->properties().isCartesianHyperbolaOfDegreeTwo()) {
      // Hyperbolas have an undefined section along-side the x axis.
      double previousT = t;
      int tries = 0;
      int maxTries = std::ceil(numberOfStepsInGradUnit *
                               CurveViewRange::k_maxNumberOfXGridUnits);
      do {
        // Try to jump out of the undefined section
        t += dir * step;
        xy = function->evaluateXYAtParameter(t, context, *subCurveIndex);
        tries++;
      } while (std::isnan(xy.y()) && tries < maxTries);
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

  cursor->moveTo(t, xy.x(), xy.y());
  return true;
}

PointOrScalar<double>
GraphControllerHelper::reloadDerivativeInBannerViewForCursorOnFunction(
    CurveViewCursor* cursor, Ion::Storage::Record record, int derivationOrder) {
  ExpiringPointer<ContinuousFunction> function =
      App::app()->functionStore()->modelForRecord(record);
  PointOrScalar<double> derivative = function->approximateDerivative<double>(
      cursor->t(), App::app()->localContext(), derivationOrder);

  constexpr size_t bufferSize = FunctionBannerDelegate::k_textBufferSize;
  char buffer[bufferSize];
  size_t numberOfChar =
      function->nameWithArgument(buffer, bufferSize, derivationOrder);
  assert(function->canDisplayDerivative());
  Preferences::PrintFloatMode mode =
      Preferences::SharedPreferences()->displayMode();
  int precision = Preferences::SharedPreferences()->numberOfSignificantDigits();
  if (function->properties().isParametric()) {
    assert(derivative.isPoint());
    Coordinate2D<double> xy = derivative.toPoint();
    Print::CustomPrintf(buffer + numberOfChar, bufferSize - numberOfChar,
                        "=(%*.*ed;%*.*ed)", xy.x(), mode, precision, xy.y(),
                        mode, precision);
  } else {
    assert(derivative.isScalar());
    /* Force derivative to 0 if cursor is at an extremum where the function is
     * differentiable. */
    if (derivationOrder == 1) {
      PointsOfInterestCache* pointsOfInterest =
          App::app()->graphController()->pointsOfInterestForRecord(record);
      if (std::isfinite(derivative.toScalar()) &&
          (pointsOfInterest->hasInterestAtCoordinates(
               cursor->x(), cursor->y(),
               Internal::Solver<double>::Interest::LocalMaximum) ||
           pointsOfInterest->hasInterestAtCoordinates(
               cursor->x(), cursor->y(),
               Internal::Solver<double>::Interest::LocalMinimum))) {
        derivative = PointOrScalar<double>(0.);
      }
    }
    Print::CustomPrintf(buffer + numberOfChar, bufferSize - numberOfChar,
                        "=%*.*ed", derivative.toScalar(), mode, precision);
  }
  if (derivationOrder == 1) {
    bannerView()->firstDerivativeView()->setText(buffer);
  } else {
    assert(derivationOrder == 2);
    bannerView()->secondDerivativeView()->setText(buffer);
  }
  bannerView()->reload();

  return derivative;
}

double GraphControllerHelper::reloadSlopeInBannerViewForCursorOnFunction(
    CurveViewCursor* cursor, Ion::Storage::Record record) {
  ExpiringPointer<ContinuousFunction> function =
      App::app()->functionStore()->modelForRecord(record);
  double slope =
      function->approximateSlope(cursor->t(), App::app()->localContext());
  constexpr size_t bufferSize = FunctionBannerDelegate::k_textBufferSize;
  char buffer[bufferSize];
  Print::CustomPrintf(
      buffer, bufferSize, "%s=%*.*ed",
      I18n::translate(I18n::Message::CartesianSlopeFormula), slope,
      Preferences::SharedPreferences()->displayMode(),
      Preferences::SharedPreferences()->numberOfSignificantDigits());
  bannerView()->slopeView()->setText(buffer);
  bannerView()->reload();
  return slope;
}

bool GraphControllerHelper::snapToInterestAndUpdateCursor(
    CurveViewCursor* cursor, double start, double end, int subCurveIndex) {
  PointOfInterest nextPointOfInterest =
      App::app()
          ->graphController()
          ->pointsOfInterestForSelectedRecord()
          ->firstPointInDirection(start, end,
                                  Internal::Solver<double>::Interest::None,
                                  subCurveIndex);
  Coordinate2D<double> nextPointOfInterestXY = nextPointOfInterest.xy();
  if (!std::isfinite(nextPointOfInterestXY.x())) {
    return false;
  }
  cursor->moveTo(nextPointOfInterest.abscissa, nextPointOfInterestXY.x(),
                 nextPointOfInterestXY.y());
  return true;
}

}  // namespace Graph
