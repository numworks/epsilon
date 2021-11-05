#include "graph_controller_helper.h"
#include "../../shared/function_banner_delegate.h"
#include "../app.h"
#include "../../shared/poincare_helpers.h"
#include <poincare/preferences.h>
#include <poincare/print.h>
#include <algorithm>

using namespace Shared;
using namespace Poincare;

namespace Graph {

bool GraphControllerHelper::privateMoveCursorHorizontally(Shared::CurveViewCursor * cursor, int direction, Shared::InteractiveCurveViewRange * range, int numberOfStepsInGradUnit, Ion::Storage::Record record, int scrollSpeed, int * subCurveIndex) {
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
  function = App::app()->functionStore()->modelForRecord(record); // Reload the expiring pointer
  double dir = (direction > 0 ? 1.0 : -1.0);
  double step = function->isAlongX() ? static_cast<double>(range->xGridUnit())/numberOfStepsInGradUnit : (tMax-tMin)/k_definitionDomainDivisor;

  bool specialConicCursorMove = false;
  if (function->isConic() && function->numberOfSubCurves() == 2) {
    assert(subCurveIndex != nullptr);
    // previousXY will be needed for conic's special horizontal cursor moves.
    specialConicCursorMove = std::isfinite(function->evaluateXYAtParameter(t, App::app()->localContext(), *subCurveIndex).x2());
    if (*subCurveIndex == 1 && function->plotType() != ContinuousFunction::PlotType::Hyperbola) {
      // On the sub curve, pressing left actually moves the cursor right
      dir *= -1.0;
    }
  }

  // Cursor's default horizontal movement
  t += dir * step * scrollSpeed;

  // If possible, round t so that f(x) matches f evaluated at displayed x
  t = FunctionBannerDelegate::getValueDisplayedOnBanner(t, App::app()->localContext(), Preferences::sharedPreferences()->numberOfSignificantDigits(), 0.05 * step, true);

  t = std::max(tMin, std::min(tMax, t));
  Coordinate2D<double> xy = function->evaluateXYAtParameter(t, App::app()->localContext(), *subCurveIndex);

  if (specialConicCursorMove && std::isnan(xy.x2())) {
    if (function->plotType() == ContinuousFunction::PlotType::Hyperbola) {
      // Hyperbolas have an undefined section along-side the x axis.
      double previousT = t;
      int tries = 0;
      int maxTries = std::ceil(numberOfStepsInGradUnit * Shared::CurveViewRange::k_maxNumberOfXGridUnits);
      do {
        // Try to jump out of the undefined section
        t += dir * step;
        xy = function->evaluateXYAtParameter(t, App::app()->localContext(), *subCurveIndex);
        tries ++;
      } while (std::isnan(xy.x2()) && tries < maxTries);
      if (tries >= maxTries || t < tMin || t > tMax) {
        // Reset to default t and xy
        t = previousT;
        xy = function->evaluateXYAtParameter(t, App::app()->localContext(), *subCurveIndex);
      }
    } else {
      /* The cursor would end up out of the conic's bounds, do not move the
       * cursor and switch to the other sub curve (with inverted dir) */
      t = tCursorPosition;
      *subCurveIndex = 1 - *subCurveIndex;
      xy = function->evaluateXYAtParameter(t, App::app()->localContext(), *subCurveIndex);
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
  Poincare::Print::customPrintf(buffer + numberOfChar, bufferSize - numberOfChar, "=%*.*ed",
    function->approximateDerivative(cursor->x(), App::app()->localContext()), Poincare::Preferences::sharedPreferences()->displayMode(), Preferences::sharedPreferences()->numberOfSignificantDigits());
  bannerView()->derivativeView()->setText(buffer);
  bannerView()->reload();
}

}
