#include "graph_controller_helper.h"
#include "../../shared/function_banner_delegate.h"
#include "../app.h"
#include "../../shared/poincare_helpers.h"
#include <poincare/preferences.h>
#include <algorithm>

using namespace Shared;
using namespace Poincare;

namespace Graph {

bool GraphControllerHelper::privateMoveCursorHorizontally(Shared::CurveViewCursor * cursor, int direction, Shared::InteractiveCurveViewRange * range, int numberOfStepsInGradUnit, Ion::Storage::Record record, int scrollSpeed) {
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
  double step = function->plotType() == ContinuousFunction::PlotType::Cartesian ? range->xGridUnit()/numberOfStepsInGradUnit : (tMax-tMin)/k_definitionDomainDivisor;
  t += dir * step * scrollSpeed;

  // If possible, round t so that f(x) matches f evaluated at displayed x
  t = FunctionBannerDelegate::getValueDisplayedOnBanner(t, App::app()->localContext(), 0.05 * step, true);

  t = std::max(tMin, std::min(tMax, t));
  Coordinate2D<double> xy = function->evaluateXYAtParameter(t, App::app()->localContext());
  cursor->moveTo(t, xy.x1(), xy.x2());
  return true;
}

void GraphControllerHelper::reloadDerivativeInBannerViewForCursorOnFunction(Shared::CurveViewCursor * cursor, Ion::Storage::Record record) {
  ExpiringPointer<ContinuousFunction> function = App::app()->functionStore()->modelForRecord(record);
  constexpr size_t bufferSize = FunctionBannerDelegate::k_maxNumberOfCharacters+PrintFloat::charSizeForFloatsWithPrecision(Preferences::LargeNumberOfSignificantDigits);
  char buffer[bufferSize];
  const char * space = " ";
  int numberOfChar = function->derivativeNameWithArgument(buffer, bufferSize);
  const char * legend = "=";
  assert(numberOfChar <= bufferSize);
  numberOfChar += strlcpy(buffer+numberOfChar, legend, bufferSize-numberOfChar);
  double y = function->approximateDerivative(cursor->x(), App::app()->localContext());
  numberOfChar += PoincareHelpers::ConvertFloatToText<double>(y, buffer + numberOfChar, bufferSize-numberOfChar, Preferences::ShortNumberOfSignificantDigits);
  assert(numberOfChar <= bufferSize);
  strlcpy(buffer+numberOfChar, space, bufferSize-numberOfChar);
  bannerView()->derivativeView()->setText(buffer);
  bannerView()->reload();
}

}
