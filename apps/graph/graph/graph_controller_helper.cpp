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
  double tCursorPosition = cursor->t();
  double t = tCursorPosition;
  double tMin = function->tMin();
  double tMax = function->tMax();
  int functionsCount = -1;
  if (((direction > 0 && std::abs(t-tMax) < DBL_EPSILON)
        || (direction < 0 && std::abs(t-tMin) < DBL_EPSILON))
      && !App::app()->functionStore()->displaysNonCartesianFunctions(&functionsCount))
  {
    jumpToLeftRightCurve(t, direction, functionsCount, record);
    return true;
  }
  Poincare::Context * context = App::app()->localContext();
  function = App::app()->functionStore()->modelForRecord(record); // Reload the expiring pointer
  double dir = (direction > 0 ? 1.0 : -1.0);

  bool specialConicCursorMove = false;
  if (function->isConic() && function->numberOfSubCurves() == 2) {
    assert(subCurveIndex != nullptr);
    // previousXY will be needed for conic's special horizontal cursor moves.
    specialConicCursorMove = std::isfinite(function->evaluateXYAtParameter(t, context, *subCurveIndex).x2());
    if (*subCurveIndex == 1 && function->plotType() != ContinuousFunction::PlotType::Hyperbola) {
      // On the sub curve, pressing left actually moves the cursor right
      dir *= -1.0;
    }
  }

  double step;
  if (function->isAlongXOrY()) {
    step = static_cast<double>(range->xGridUnit())/numberOfStepsInGradUnit;
    double slopeMultiplicator = 1.0;
    if (function->canDisplayDerivative()) {
      // Use the local derivative to slow down the cursor's step if needed
      double slope = function->approximateDerivative(t, context, *subCurveIndex);
      // If yGridUnit is twice xGridUnit, visible slope is halved
      slope *= range->xGridUnit() / range->yGridUnit();
      /* Assuming the curve is a straight line of slope s. To move the cursor at a
      * fixed distance t along the line, the actual x-axis distance needed is
      * t' = t * cos(θ) with θ the angle between the line and the x-axis.
      * We also have tan(θ) = (s * t) / t = s
      * As a result, t' = t * cos(atan(s)) = t / sqrt(1 + s^2) */
      slopeMultiplicator /= std::sqrt(1.0 + slope*slope);
      // Add a sqrt(2) factor so that y=x isn't slowed down
      slopeMultiplicator *= std::sqrt(2.0);
      /* Cap the scroll speed reduction to be able to cross vertical asymptotes
       * Using pixelWidth / (step * static_cast<double>(scrollSpeed)) as cap
       * ensures that t moves at least by one pixel.
       */
      slopeMultiplicator = std::max(pixelWidth / (step * static_cast<double>(scrollSpeed)), slopeMultiplicator);
    }
    // Cursor's default horizontal movement
    t += dir * step * slopeMultiplicator * static_cast<double>(scrollSpeed);
    assert(std::round(static_cast<float>(t)/pixelWidth) != std::round(static_cast<float>(tCursorPosition)/pixelWidth));
    /* assert that it moved at least of 1 pixel.
     * round(t/pxWidth) is used by CurveView to compute the cursor's position.
     */

    // Use a pixel width as a margin, ensuring t mostly stays at the same pixel
    if (std::fabs(static_cast<float>(tCursorPosition)) >= pixelWidth && ((dir < 0) != (tCursorPosition < 0)) && std::fabs(static_cast<float>(t)) < pixelWidth) {
      // Round t to 0 if it is going into that direction, and is close enough
      t = 0.0;
    } else {
      // Round t to a simpler value, displayed at the same index
      double magnitude = std::pow(10.0, Poincare::IEEE754<double>::exponentBase10(pixelWidth));
      t = magnitude * std::round(t / magnitude);
      // Also round t so that f(x) matches f evaluated at displayed x
      t = FunctionBannerDelegate::getValueDisplayedOnBanner(t, context, Preferences::sharedPreferences()->numberOfSignificantDigits(), pixelWidth, false);
    }
  } else {
    /* If function is not along X or Y, the cursor speed along t should not
     * depend on pixelWidth since the t interval can be very small even if the
     * pixel width is very large. */
    step = (tMax-tMin)/k_definitionDomainDivisor;
    t += dir * step * scrollSpeed;
    // If possible, round t so that f(x) matches f evaluated at displayed x
    t = FunctionBannerDelegate::getValueDisplayedOnBanner(t, App::app()->localContext(), Preferences::sharedPreferences()->numberOfSignificantDigits(), 0.05 * step, true);
  }
  // t must have changed
  assert(tCursorPosition != t);

  t = std::max(tMin, std::min(tMax, t));
  int subCurveIndexValue = subCurveIndex == nullptr ? 0 : *subCurveIndex;
  Coordinate2D<double> xy = function->evaluateXYAtParameter(t, context, subCurveIndexValue);

  if (specialConicCursorMove && std::isnan(xy.x2())) {
    if (function->plotType() == ContinuousFunction::PlotType::Hyperbola) {
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
      t = tCursorPosition;
      *subCurveIndex = 1 - *subCurveIndex;
      xy = function->evaluateXYAtParameter(t, context, *subCurveIndex);
    }
  }

  cursor->moveTo(t, xy.x1(), xy.x2());
  return true;
}

void GraphControllerHelper::reloadDerivativeInBannerViewForCursorOnFunction(Shared::CurveViewCursor * cursor, Ion::Storage::Record record) {
  ExpiringPointer<ContinuousFunction> function = App::app()->functionStore()->modelForRecord(record);
  constexpr size_t bufferSize = FunctionBannerDelegate::k_textBufferSize;
  char buffer[bufferSize];
  int numberOfChar = function->derivativeNameWithArgument(buffer, bufferSize);
  assert(function->canDisplayDerivative());
  Poincare::Print::customPrintf(buffer + numberOfChar, bufferSize - numberOfChar, "=%*.*ed",
    function->approximateDerivative(cursor->x(), App::app()->localContext()), Poincare::Preferences::sharedPreferences()->displayMode(), Preferences::sharedPreferences()->numberOfSignificantDigits());
  bannerView()->derivativeView()->setText(buffer);
  bannerView()->reload();
}

}
