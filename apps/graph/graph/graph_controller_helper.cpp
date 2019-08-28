#include "graph_controller_helper.h"
#include "../../shared/function_banner_delegate.h"
#include "../app.h"
#include "../../shared/poincare_helpers.h"
#include <poincare/preferences.h>

using namespace Shared;
using namespace Poincare;

namespace Graph {

bool GraphControllerHelper::privateMoveCursorHorizontally(Shared::CurveViewCursor * cursor, int direction, Shared::InteractiveCurveViewRange * range, int numberOfStepsInGradUnit, Ion::Storage::Record record) {
  ExpiringPointer<CartesianFunction> function = App::app()->functionStore()->modelForRecord(record);
  double tCursorPosition = cursor->t();
  double t = tCursorPosition;
  double dir = (direction > 0 ? 1.0 : -1.0);
  CartesianFunction::PlotType type = function->plotType();
  if (type == CartesianFunction::PlotType::Cartesian) {
    t+= dir * range->xGridUnit()/numberOfStepsInGradUnit;
  } else if (type == CartesianFunction::PlotType::Polar) {
    t += dir; //TODO LEA delt
  } else {
    assert(type == CartesianFunction::PlotType::Parametric);
    t += dir; //TODO LEA delt
  }

  Coordinate2D<double> xy = function->evaluateXYAtParameter(t, App::app()->localContext());
  cursor->moveTo(t, xy.x(), xy.y());
  return true;
}

void GraphControllerHelper::reloadDerivativeInBannerViewForCursorOnFunction(Shared::CurveViewCursor * cursor, Ion::Storage::Record record) {
  ExpiringPointer<CartesianFunction> function = App::app()->functionStore()->modelForRecord(record);
  constexpr size_t bufferSize = FunctionBannerDelegate::k_maxNumberOfCharacters+PrintFloat::bufferSizeForFloatsWithPrecision(Preferences::LargeNumberOfSignificantDigits);
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
