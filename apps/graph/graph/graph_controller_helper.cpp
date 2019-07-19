#include "graph_controller_helper.h"
#include "../../shared/function_banner_delegate.h"
#include "../app.h"
#include "../../constant.h"
#include "../../shared/poincare_helpers.h"

using namespace Shared;
using namespace Poincare;

namespace Graph {

bool GraphControllerHelper::privateMoveCursorHorizontally(Shared::CurveViewCursor * cursor, int direction, Shared::InteractiveCurveViewRange * range, int numberOfStepsInGradUnit, Ion::Storage::Record record) {
  ExpiringPointer<CartesianFunction> function = App::app()->functionStore()->modelForRecord(record);
  double xCursorPosition = cursor->x();
  double x = direction > 0 ? xCursorPosition + range->xGridUnit()/numberOfStepsInGradUnit : xCursorPosition -  range->xGridUnit()/numberOfStepsInGradUnit;
  double y = function->evaluateAtAbscissa(x, App::app()->localContext());
  cursor->moveTo(x, y);
  return true;
}

void GraphControllerHelper::reloadDerivativeInBannerViewForCursorOnFunction(Shared::CurveViewCursor * cursor, Ion::Storage::Record record) {
  ExpiringPointer<CartesianFunction> function = App::app()->functionStore()->modelForRecord(record);
  constexpr size_t bufferSize = FunctionBannerDelegate::k_maxNumberOfCharacters+PrintFloat::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits);
  char buffer[bufferSize];
  const char * space = " ";
  int numberOfChar = function->derivativeNameWithArgument(buffer, bufferSize, CartesianFunction::Symbol());
  const char * legend = "=";
  numberOfChar += strlcpy(buffer+numberOfChar, legend, bufferSize-numberOfChar);
  double y = function->approximateDerivative(cursor->x(), App::app()->localContext());
  numberOfChar += PoincareHelpers::ConvertFloatToText<double>(y, buffer + numberOfChar, bufferSize-numberOfChar, Constant::ShortNumberOfSignificantDigits);
  strlcpy(buffer+numberOfChar, space, bufferSize-numberOfChar);
  bannerView()->derivativeView()->setText(buffer);
  bannerView()->reload();
}

}
