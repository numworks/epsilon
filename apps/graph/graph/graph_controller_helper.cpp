#include "graph_controller_helper.h"
#include "../../constant.h"
#include "../../shared/poincare_helpers.h"

using namespace Shared;
using namespace Poincare;

namespace Graph {

bool GraphControllerHelper::privateMoveCursorHorizontally(Shared::CurveViewCursor * cursor, int direction, Shared::InteractiveCurveViewRange * range, int numberOfStepsInGradUnit, Shared::StorageFunction * function, Shared::TextFieldDelegateApp * app, float cursorTopMarginRatio, float cursorRightMarginRatio, float cursorBottomMarginRatio, float cursorLeftMarginRatio) {
  double xCursorPosition = cursor->x();
  double x = direction > 0 ? xCursorPosition + range->xGridUnit()/numberOfStepsInGradUnit : xCursorPosition -  range->xGridUnit()/numberOfStepsInGradUnit;
  double y = function->evaluateAtAbscissa(x, app->localContext());
  cursor->moveTo(x, y);
  range->panToMakePointVisible(x, y, cursorTopMarginRatio, cursorRightMarginRatio, cursorBottomMarginRatio, cursorLeftMarginRatio);
  return true;
}

void GraphControllerHelper::reloadDerivativeInBannerViewForCursorOnFunction(Shared::CurveViewCursor * cursor, StorageCartesianFunction * function, TextFieldDelegateApp * app) {
  constexpr size_t bufferSize = FunctionBannerDelegate::k_maxNumberOfCharacters+PrintFloat::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits);
  char buffer[bufferSize];
  const char * space = "                  ";
  int numberOfChar = function->derivativeNameWithArgument(buffer, bufferSize, 'x');
  const char * legend = "=";
  numberOfChar += strlcpy(buffer+numberOfChar, legend, bufferSize-numberOfChar);
  double y = function->approximateDerivative(cursor->x(), app->localContext());
  numberOfChar += PoincareHelpers::ConvertFloatToText<double>(y, buffer + numberOfChar, bufferSize-numberOfChar, Constant::ShortNumberOfSignificantDigits);
  strlcpy(buffer+numberOfChar, space, bufferSize-numberOfChar);
  buffer[k_maxDigitLegendLength+6] = 0;
  bannerView()->setLegendAtIndex(buffer, 2);
}

}
