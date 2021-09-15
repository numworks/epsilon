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

bool GraphControllerHelper::privateMoveCursorHorizontally(Shared::CurveViewCursor * cursor, int direction, Shared::InteractiveCurveViewRange * range, int numberOfStepsInGradUnit, Ion::Storage::Record record, int scrollSpeed, int * secondaryCurveIndex) {
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

  if (function->plotType() == ContinuousFunction::PlotType::Ellipse || function->plotType() == ContinuousFunction::PlotType::Circle || (function->plotType() == ContinuousFunction::PlotType::Parabola && function->hasTwoCurves())) {
    assert(secondaryCurveIndex != nullptr && function->hasTwoCurves());
    if (*secondaryCurveIndex == 1) {
      dir *= -1.0;
    }
    Coordinate2D<double> xy = function->evaluateXYAtParameter(t, App::app()->localContext(), *secondaryCurveIndex);
    if (std::isfinite(xy.x2())) {
      double t2 = t + dir * step * scrollSpeed;
      xy = function->evaluateXYAtParameter(t2, App::app()->localContext(), *secondaryCurveIndex);
      if (std::isnan(xy.x2())) {
        *secondaryCurveIndex = 1 - *secondaryCurveIndex;
        t -= dir * step * scrollSpeed;
      }
    }
  }
  if (function->plotType() == ContinuousFunction::PlotType::Hyperbola || function->plotType() == ContinuousFunction::PlotType::Circle) {
    Coordinate2D<double> xy = function->evaluateXYAtParameter(t, App::app()->localContext(), *secondaryCurveIndex);
    if (std::isfinite(xy.x2())) {
      double t2 = t + dir * step * scrollSpeed;
      xy = function->evaluateXYAtParameter(t2, App::app()->localContext(), *secondaryCurveIndex);
      int tries = 0;
      while (std::isnan(xy.x2()) && tries < 20) {
        tries ++;
        t2 += dir * step * scrollSpeed;
        xy = function->evaluateXYAtParameter(t2, App::app()->localContext(), *secondaryCurveIndex);
      }
      t = t2 - dir * step * scrollSpeed;
      t = std::max(tMin, std::min(tMax, t));
    }
  }
  t += dir * step * scrollSpeed;

  // If possible, round t so that f(x) matches f evaluated at displayed x
  t = FunctionBannerDelegate::getValueDisplayedOnBanner(t, App::app()->localContext(), Preferences::sharedPreferences()->numberOfSignificantDigits(), 0.05 * step, true);

  t = std::max(tMin, std::min(tMax, t));
  Coordinate2D<double> xy = function->evaluateXYAtParameter(t, App::app()->localContext(), *secondaryCurveIndex);
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
